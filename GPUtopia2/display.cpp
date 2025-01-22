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
		ImGuiIO& io = ImGui::GetIO();
		ImGui::Begin("Main View", nullptr, ImGuiWindowFlags_HorizontalScrollbar);
		static GLuint textureID;
		static bool updateTexture = false;
		static bool refreshDefaultArguments = true;
		static bool refreshFractalArguments = true;
		static ImVec2 mainViewportSize = ImGui::GetContentRegionAvail();
		static clFractal cf;
		static clFractal cf_old;
		static fractalNavigationParameters nav;
		static std::vector<color> textureColors(cf.image.size.x * cf.image.size.y);
		static std::vector<color> vec_img_f_offset(cf.image.size.x * cf.image.size.y, 0);
		static bool imgBlocked = false;
		static bool needCLFractal = true;
		static bool showCoreError = false;
		static clCore core;
		// only run at startup, maybe move somewhere else?
		if (needCLFractal)
		{
			cf.makeCLCode();
			core.compileNewKernel(cf);
			if (core.compileError == CL_SUCCESS)
			{
				needCLFractal = false;
			}
			else
			{
				needCLFractal = true;
			}
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
		if (cf.buildKernel and core.errSum() == 0)
		{
			std::cout << "Need a new kernel, compiling it now.\n - requesting new texture\n - requesting new image\n";
			core.compileNewKernel(cf);
			cf.status.runKernel = true;
		}
		if (core.errSum())
		{
			core.stop = true;
			show_cl_error_window(cf, core, font_mono);
		}		static paramCollector params_old = cf.params;
		static clFractalImage img_settings_old = cf.image;
		formulaSettingsWindow(cf, core);
		imageSettingsWindow(cf, textureColors);
		flameRenderSettingsWindow(cf);
		infoWindow(cf, nav, font_mono);
		static int waitCounter = 0;
		static bool force_img_update = false;
		// only order rerun of imgKernel if flameRenderSettings change
		if (cf.flameRenderSettings != cf_old.flameRenderSettings and !cf.running()) {
			std::cout << "Fractal's flameRenderSettings changed, demanding new run of imgKernel.\n";
			cf.status.runImgKernel = true;
			cf_old.flameRenderSettings = cf.flameRenderSettings;
		}
		// 
		if (cf != cf_old) { //}&& waitCounter == 0) {
			std::cout << "Parameters changed, updating cf.params.\n - requesting new Texture\n - requesting new image\n";
			needCLFractal = false;
			cf.image.updateComplexSubplane();
			core.setDefaultArguments(cf);
			core.setKernelFractalArgs(cf);
			cf.image.resetStatus();
			cf.status.runKernel = true;
			cf.status.done = false;
			cf_old = cf;
		}

		glViewport(0, 0, mainViewportSize.x, mainViewportSize.y);
		// first draw the image
		static bool running = false;
		static std::jthread jt;
		if (cf.status.runKernel and !cf.running() and cf.image.current_sample_count < cf.image.target_sample_count) {
			if (cf.image.current_sample_count == 0)
			{
				cf.image.next_update_sample_count = 1;
				core.setDefaultArguments(cf);
				std::cout << "kernel currently has " << core.kernelArgumentCount << " arguments.'n";
				core.setKernelFractalArgs(cf);
			}
			cf.status.kernelRunning = true;
			jt = std::jthread(&runKernelAsync, std::ref(cf), std::ref(core));
			jt.detach();
			cf.status.runKernel = false;
		}

		if (cf.status.runImgKernel and !cf.running()) {
			std::cout << "Need a new image, setting kernel args and running kernel.\n";
			std::cout << "sampling info is (" << cf.image.current_sample_count << ", " << cf.image.target_sample_count << ")\n";
			cf.status.imgKernelRunning = true;
			jt = std::jthread(&runImgKernelAsync, std::ref(cf), std::ref(core));
			jt.detach();
			cf.status.runImgKernel = false;
		}
		// check current image quality
		if (!cf.running() and !cf.status.done and core.errSum() == 0)
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
			std::cout << "Need a new texture, deleting old and remaking using the new image.\n";
			glDeleteTextures(1, &textureID);
			std::cout << "IMAGE SIZE: " << cf.image.size.x << ", " << cf.image.size.y << "(" << cf.image.size.x * cf.image.size.y << ")\n";
			std::cout << textureColors.size() << "\n";
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
			textureColors = vec_img_f_offset;
			imgBlocked = false;
		}
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && nav.draggingRotate)
		{
			nav.draggingRotate = false;
			// calculate coordinate of pixel in the middle of the displayed image
			cf.image.angle += nav.dragAngle;
			cf.image.rotation = cf.image.rotation * nav.dragRotation;
			textureColors = vec_img_f_offset;
			imgBlocked = false;
		}
		ImGui::End();
		static char mainViewStr[] = "Main View";
		ImGui::Begin(mainViewStr, nullptr, ImGuiWindowFlags_HorizontalScrollbar);
		ImGui::Image((void*)(intptr_t)textureID, ImVec2(
			cf.image.size.x, cf.image.size.y)); // , texturesize);
		static std::string fileName;
		if (ImGui::Button("open"))
		{
			openFileDialog(fileName);
		}
		ImGui::Text(fileName.c_str());
		
		ImGui::End();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};