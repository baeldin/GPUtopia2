#include "display.h"


void prepTexture(GLuint& texture)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

	// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
}


// color the texture
void makeTexture(GLuint& texture, const int sizeX, const int sizeY, std::vector<color>& imgData)
{
	prepTexture(texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, sizeX, sizeY, 0, GL_RGBA, GL_FLOAT, imgData.data());
}

// refresh image
void refreshTexture(GLuint& texture, const int sizeX, const int sizeY, std::vector<color>& imgData)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, sizeX, sizeY, 0, GL_RGBA, GL_FLOAT, imgData.data());
}


namespace mainView
{
	void mainViewPort(ImFont* font_mono)
	{
		static GLuint textureID;
		static bool updateTexture = false;
		static bool refreshDefaultArguments = true;
		static bool refreshFractalArguments = true;
		static ImVec2 mainViewportSize = ImGui::GetContentRegionAvail();
		static clFractal cf;
		static clFractal cf_old;
		static std::vector<clFractalMinimal> history; // = { cf.toExport() }; // history vector
		static int historyIndex = -1;
		static bool undone = false;
		static bool redone = false;
		static fractalNavigationParameters nav;
		static std::vector<color> textureColors(cf.image.size.x * cf.image.size.y);
		static std::vector<color> vec_img_f_offset(cf.image.size.x * cf.image.size.y, 0.f);
		static bool imgBlocked = false;
		static bool startup = true;
		static bool showCoreError = false;
		static clCore core;
		if (core.imgKernel.need)
			core.compileImgKernel();
		// only run at startup, maybe move somewhere else?
		while (startup)
		{
			// cf.verbosity = 2;
			cf.makeCLCode(NEW_FILES);
			cf.buildKernel = true;
			std::cout << "Doing the starting thing " << startup << "\n";
			startup = false;
			cf_old.image.zoom = -1.; // just ANY difference will do for startup
		}
		// check if main viewport needs resizing
		if (mainViewportSize.x != ImGui::GetContentRegionAvail().x ||
			mainViewportSize.y != ImGui::GetContentRegionAvail().y)
		{
			mainViewportSize = ImGui::GetContentRegionAvail();
			mainViewportSize.y < 1 ? mainViewportSize.y = 1 : mainViewportSize.y;
		}
		// check if image size changed, if so, resize the texture too
		if (cf_old.image.size != cf.image.size and !cf.running())
		{
			cf.status.runImgKernel = true;
			textureColors.resize(cf.image.size.x * cf.image.size.y);
			vec_img_f_offset.resize(cf.image.size.x * cf.image.size.y);
			cf_old.image = cf.image;
		}
		// check if a kernel rebuild is needed
		if (cf.buildKernel and core.fractalKernel.errors.sum() == 0)
		{
			core.compileFractalKernel(cf.fullCLcode);
			cf.buildKernel = false;
			cf.status.runKernel = true;
		}
		if (core.imgKernel.errors.sum() != 0)
		{
			core.stop = true;
			show_cl_error_window(cf, core, font_mono);
		}
		else if (core.fractalKernel.errors.sum() != 0)
		{
			core.stop = true;
			show_cl_error_window(cf, core, font_mono);
		}
		ImGuiIO& io = ImGui::GetIO();
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.
				if (ImGui::MenuItem("Open"))
				{
					std::string path;
					bool success = false;
					openFileDialog(path, success);
					if (success)
					{
						std::ifstream inFile(path);
					    std:cout << path << "\n";
						std::string jsonStr;
						inFile >> jsonStr;
						json back_json = json::parse(jsonStr);
						auto cfm = back_json.get<clFractalMinimal>();
						cf = clFractal(cfm);
						cf.makeCLCode(SAME_FILES);
						cf.image.size.x = 800;
						cf.image.size.y = 600;
						// cf.buildKernel = true;
						core.resetCore();
						core.compileFractalKernel(cf.fullCLcode);
					}
				}
				if (ImGui::MenuItem("Save As"))
				{
					bool success = false;
					std::string path;
					saveFileDialog(path, success);
					if (success)
					{
						json json = cf.toExport();
						std::ofstream outFile(path);
						outFile << json.dump();
						outFile.close();
					}
				}
				if (ImGui::MenuItem("Export Image"))
				{
					bool success = false;
					std::string fileName;
					saveFileDialog(fileName, success);
					if (success)
						save_to_png(textureColors, cf.image.size.x, cf.image.size.y, (char*)fileName.c_str());
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo"))
				{
					if (historyIndex > 0)
					{
						historyIndex--;
						cf = clFractal(history[historyIndex]);
						if (cf.fractalCLFragmentFile != cf_old.fractalCLFragmentFile ||
							cf.coloringCLFragmentFile != cf_old.coloringCLFragmentFile)
						{
							cf.makeCLCode();
							// cf.buildKernel = true;
							core.resetCore();
							core.compileFractalKernel(cf.fullCLcode);
						}
						cf.image.resetStatus();
						cf.status.runKernel = true;
						cf.timings.erase(cf.timings.begin(), cf.timings.end());
						cf.status.done = false;
						cf_old = cf;
						std::cout << "History index = " << historyIndex << " and length of history vector is " << history.size() << "\n";
						redone = false;
						undone = true;
					}
				}
				if (ImGui::MenuItem("Redo"))
				{
					if (historyIndex < history.size() - 1)
					{
						historyIndex++;
						cf = clFractal(history[historyIndex]);
						if (cf.fractalCLFragmentFile != cf_old.fractalCLFragmentFile ||
							cf.coloringCLFragmentFile != cf_old.coloringCLFragmentFile)
						{
							cf.makeCLCode();
							// cf.buildKernel = true;
							core.resetCore();
							core.compileFractalKernel(cf.fullCLcode);
						}
						cf.image.resetStatus();
						cf.status.runKernel = true;
						cf.timings.erase(cf.timings.begin(), cf.timings.end());
						cf.status.done = false;
						cf_old = cf;
						std::cout << "History index = " << historyIndex << " and length of history vector is " << history.size() << "\n";
						undone = false;
						redone = true;
					}
				}
				if (ImGui::MenuItem("TODO: Copy"))
				{
					clFractalMinimal exp = cf.toExport();

				}
				if (ImGui::MenuItem("TODO: Paste"))
				{
					// redo
				}
				if (ImGui::MenuItem("TODO: Copy Image"))
				{
					// redo
				}
				ImGui::EndMenu();

			}
			ImGui::EndMainMenuBar();
		}
		ImGui::Begin("Main View", nullptr, ImGuiWindowFlags_HorizontalScrollbar);
		static paramCollector params_old = cf.params;
		static clFractalImage img_settings_old = cf.image;
		formulaSettingsWindow(cf, core);
		imageSettingsWindow(cf);
		flameRenderSettingsWindow(cf);
		infoWindow(cf, nav, font_mono);
		static int waitCounter = 0;
		static bool force_img_update = false;
		// only order rerun of imgKernel if flameRenderSettings change
		if (cf.flameRenderSettings != cf_old.flameRenderSettings and !cf.running()) {
			cf.status.runImgKernel = true;
			if (historyIndex < history.size() - 1)
			{
				std::cout << "We are not at the end of history, but the fractal was changed:\n";
				const int historyEnd = history.size();
				for (int ii = historyIndex; ii < historyEnd; ii++)
				{
					std::cout << "Popping history[" << ii << "]\n";
					history.pop_back();
				}
				historyIndex--;
				std::cout << "##################################\nHistory index = " << historyIndex << " and length of history vector is " << history.size() << "\n";
			}
			history.push_back(cf.toExport());
			historyIndex++;
			undone = false;
			redone = false;
			std::cout << "History index = " << historyIndex << " and length of history vector is " << history.size() << "\n";
			cf_old.flameRenderSettings = cf.flameRenderSettings;
		}
		// 
		if (cf != cf_old) { //}&& waitCounter == 0) {
			if (cf.newCLFragmentQueued())
			{
				if (cf.newFractalCLFragmentQueued())
					cf.popFractalCLFragmentQueue();
				if (cf.newColoringCLFragmentQueued())
					cf.popColoringCLFragmentQueue();
				cf.makeCLCode(NEW_FILES);
				core.resetCore();
				core.compileFractalKernel(cf.fullCLcode);
			}
			if (cf.useDouble != cf_old.useDouble)
			{
				cf.makeCLCode(SAME_FILES);
				core.resetCore();
				core.compileFractalKernel(cf.fullCLcode);
			}
			cf.image.updateComplexSubplane();
			core.setDefaultFractalArguments(cf);
			core.setFractalParameterArgs(cf);
			cf.image.resetStatus();
			cf.status.runKernel = true;
			cf.timings.erase(cf.timings.begin(), cf.timings.end());
			cf.status.done = false;
			if (historyIndex < history.size() - 1)
			{
				std::cout << "We are not at the end of history, but the fractal was changed:\n";
				const int historyEnd = history.size();
				for (int ii = historyIndex; ii < historyEnd; ii++)
				{
					std::cout << "Popping history[" << ii << "]\n";
					history.pop_back();
				}
				historyIndex--;
				std::cout << "##################################\nHistory index = " << historyIndex << " and length of history vector is " << history.size() << "\n";
			}
			history.push_back(cf.toExport());
			historyIndex++;
			undone = false;
			redone = false;
			std::cout << "##################################\nHistory index = " << historyIndex << " and length of history vector is " << history.size() << "\n";
			cf_old = cf;
		}
		glViewport(0, 0, mainViewportSize.x, mainViewportSize.y);
		// first draw the image
		static bool running = false;
		static std::jthread jt;
		if (cf.status.runKernel and !cf.running() and cf.image.current_sample_count < cf.image.target_sample_count) {
			if (cf.image.current_sample_count == 0) // if we are restarting
			{
				core.setDefaultFractalArguments(cf);
				core.setFractalParameterArgs(cf);
			}
			cf.status.runKernel = false; 
			cf.status.kernelRunning = true;
			jt = std::jthread(&runFractalKernelAsync, std::ref(cf), std::ref(core));
			jt.detach();
		}

		if (cf.status.runImgKernel and !cf.running()) {
			cf.status.imgKernelRunning = true;
			jt = std::jthread(&runImgKernelAsync, std::ref(cf), std::ref(core));
			jt.detach();
			cf.status.runImgKernel = false;
		}
		// check current image quality
		if (!cf.running() and !cf.status.done and core.fractalKernel.errors.sum() == 0)
		{
			if (cf.image.current_sample_count == cf.image.next_update_sample_count)
			{
				cf.image.next_update_sample_count = 2* cf.image.next_update_sample_count;
				cf.image.next_update_sample_count = cf.image.next_update_sample_count > cf.image.target_sample_count ? 
					cf.image.target_sample_count : cf.image.next_update_sample_count;
				cf.status.runImgKernel = true;
			}
			else
			{
				cf.status.runKernel = true;
			}
		}
		if (cf.status.updateImage and !cf.running())
		{
			//if (waitCounter > 2)
			//{
				if (cf.image.size.x * cf.image.size.y != textureColors.size())
				{
					textureColors.resize(cf.image.size.x * cf.image.size.y);
					vec_img_f_offset.resize(cf.image.size.x* cf.image.size.y);
				}
				core.getImg(textureColors, cf);
				updateTexture = true;
				waitCounter = 0;
			//}
			//waitCounter++;
		}
		// create texture from the image
		if (updateTexture) {
			glDeleteTextures(1, &textureID);
			makeTexture(textureID, cf.image.size.x, cf.image.size.y, textureColors);
			updateTexture = false;
		}
		if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		{
			if (io.KeyCtrl)
			{
				imgBlocked = true;
				dragPan(cf, nav, io);
				offsetImageInBox(textureColors, vec_img_f_offset, cf, nav);
				refreshTexture(textureID, cf.image.size.x, cf.image.size.y, vec_img_f_offset);
			}
			else if (io.KeyShift)
			{
				imgBlocked = true;
				dragZoom(cf, nav, io);
				nav.dragStart.x = (int)((io.MousePos.x - ImGui::GetCursorScreenPos().x - ImGui::GetMouseDragDelta(0).x)); // / zoom_factor);
				nav.dragStart.y = (int)((io.MousePos.y - ImGui::GetCursorScreenPos().y - ImGui::GetMouseDragDelta(0).y)); // / zoom_factor);
				nav.newImgCenter = imgCoordinateCenterAfterZoom(cf, nav);
				nav.newSubplaneCenter = cf.image.image2complex(nav.newImgCenter);
				zoomImageInBox(textureColors, vec_img_f_offset, cf, nav);
				refreshTexture(textureID, cf.image.size.x, cf.image.size.y, vec_img_f_offset);
			}
			else if (io.KeyAlt)
			{
				imgBlocked = true;
				nav.dragStart.x = (int)((io.MousePos.x - ImGui::GetCursorScreenPos().x - ImGui::GetMouseDragDelta(0).x)); // / zoom_factor);
				nav.dragStart.y = (int)((io.MousePos.y - ImGui::GetCursorScreenPos().y - ImGui::GetMouseDragDelta(0).y)); // / zoom_factor);
				dragRotate(cf, nav, io);
				rotateImageInBox(textureColors, vec_img_f_offset, cf, nav);
				refreshTexture(textureID, cf.image.size.x, cf.image.size.y, vec_img_f_offset);
			}
		}
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && nav.draggingCenter)
		{
			nav.draggingCenter = false;
			nav.centerOffset = get_complex_offset(nav.dragOffset.x, nav.dragOffset.y, cf);
			cf.image.center.x += nav.centerOffset.x;
			cf.image.center.y += nav.centerOffset.y;
			textureColors = vec_img_f_offset;
			imgBlocked = false;
		}
		static int imgDisplayCenterX = 0;
		static int imgDisplayCenterY = 0;
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && nav.draggingZoom)
		{
			nav.draggingZoom = false;
			// calculate coordinate of pixel in the middle of the displayed image
			cf.image.center = nav.newSubplaneCenter;
			cf.image.zoom *= nav.dragZoomFactor;
			std::cout << "cf.image.center.x " << cf.image.center.x << "\n";
			std::cout << "cf.image.center.y " << cf.image.center.y << "\n";
			std::cout << "cf.image.zoom " << cf.image.zoom << "\n";
			textureColors = vec_img_f_offset;
			imgBlocked = false;
		}
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && nav.draggingRotate)
		{
			nav.draggingRotate = false;
			// calculate coordinate of pixel in the middle of the displayed image
			cf.image.angle += 180. / CL_M_PI * nav.dragAngle;
			textureColors = vec_img_f_offset;
			imgBlocked = false;
		}
		ImGui::End();
		static char mainViewStr[] = "Main View";
		ImGui::Begin(mainViewStr, nullptr, ImGuiWindowFlags_HorizontalScrollbar);
		ImGui::Image((void*)(intptr_t)textureID, ImVec2(
		cf.image.size.x, cf.image.size.y)); 
		ImGui::End();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};