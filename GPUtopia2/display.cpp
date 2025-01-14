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

void offsetImageInBox(const std::vector<color>& image, std::vector<color>& offsetImage, const int dx, const int dy, const int imgWidth, const int imgHeight)
{
	// move the pixels within the image to simulate moving out outside the frame, replace black
	// pixels with black.
	int maxIndex = imgWidth * imgHeight;
	std::fill(offsetImage.begin(), offsetImage.end(), 0.f);
	for (int y = 0; y < imgHeight; y++)
	{
		for (int x = 0; x < imgWidth; x++)
		{
			int pixelIndexOld = y * imgWidth + x;
			int pixelIndexNew = (y + dy) * imgWidth + x + dx;
			// check if pixelIndexNew is out of bounds:
			if (y + dy >= 0 && x + dx >= 0 && y + dy < imgHeight && x + dx < imgWidth)
				//std::cout << x << " " << y << " " << dx << " " << dy << " " << pixelIndexOld << " " << pixelIndexNew << "\n";
			{
				offsetImage[pixelIndexNew] = image[pixelIndexOld];
			}
		}
	}
}

void zoomImageInBox(const std::vector<color>& image, std::vector<color>& offsetImage, const float dragZoomFactor,
	const int centerX, const int centerY, const int imgWidth, const int imgHeight)
{
	/* zoom into the image using a pixel coordinate center and zoom factor */
	int maxIndex = imgWidth * imgHeight;
	std::fill(offsetImage.begin(), offsetImage.end(), 0.f);
	for (int y = 0; y < imgHeight; y++)
	{
		for (int x = 0; x < imgWidth; x++)
		{
			// find out which old pixel should be mapped to the zoomed image
			int pixelIndexNew = y * imgWidth + x;
			int oldX = centerX - (centerX - x) / dragZoomFactor;
			int oldY = centerY - (centerY - y) / dragZoomFactor;
			int pixelIndexOld = oldY * imgWidth + oldX;
			if (oldY >= 0 && oldX >= 0 && oldY < imgHeight && oldX < imgWidth) // check inf inside:
			{
				offsetImage[pixelIndexNew] = image[pixelIndexOld];
			}
		}
	}
}

// fill vector with some colors
void drawBogusImg(std::vector<color>& img, int width, int height)
{
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			img[y * width + x].r = 0;
			img[y * width + x].g = (float)x / width;
			img[y * width + x].b = (float)y / height;
		}
	}
}

Complex<float> get_complex_offset(const int dx, const int dy, const clFractal& cf)
{
	const float dxFloat = (float)dx * cf.image.zoom;
	const float dyFloat = (float)dy * cf.image.zoom;
	const double xRange = cf.image.complexSubplane.z / cf.image.zoom; // max(cf.image.complexSubplane.z * cf.image.aspectRatio, cf.image.complexSubplane.z) / cf.image.zoom;
	const double yRange = cf.image.complexSubplane.w / cf.image.zoom; // max(cf.image.complexSubplane.w, cf.image.complexSubplane.z) / cf.image.zoom;
	double xOffset = -dxFloat / cf.image.size.x * xRange;
	double yOffset = dyFloat / cf.image.size.y * yRange;
	return Complex<float>(xOffset, yOffset) * cf.image.rotation;
}

Complex<float> get_complex_coord(const float x_shifted, const float y_shifted, const clFractal& cf)
{
	const double xRange = cf.image.complexSubplane.z / cf.image.zoom; // max(2.f * pos.span * cf.image.aspectRatio, 2.f * pos.span) / pos.magn;
	const double yRange = cf.image.complexSubplane.w / cf.image.zoom; // max(2.f * pos.span / cf.image.aspectRatio, 2.f * pos.span) / pos.magn;
	const float x_shifted_new = (x_shifted + 0.5 - 0.5 * cf.image.size.x) / cf.image.size.x * xRange;
	const float y_shifted_new = (y_shifted + 0.5 - 0.5 * cf.image.size.y) / cf.image.size.y * yRange;
	Complex<float> shifted_new = Complex(x_shifted_new, y_shifted_new) * cf.image.rotation;
	shifted_new.x += cf.image.complexSubplane.x;
	shifted_new.y += cf.image.complexSubplane.y;
	return shifted_new;
}

namespace mainView
{
	void mainViewPort()
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
		static std::vector<color> textureColors(cf.image.size.x * cf.image.size.y);
		static std::vector<color> vec_img_f_offset(cf.image.size.x * cf.image.size.y, 0);
		static bool imgBlocked = false;
		static bool draggingCenter = false;
		static bool draggingZoom = false;
		static float dragZoomFactor = 1.;
		static int dragStartX = 0;
		static int dragStartY = 0;

		static int dxCenter = 0;
		static int dyCenter = 0;
		static bool needCLFractal = true;
		static clCore core;
		// track coursor position
		static ImVec2 coursorPos;
		coursorPos = ImGui::GetCursorScreenPos();

		// only run at startup, maybe move somewhere else?
		if (needCLFractal)
		{
			cf.makeCLCode();
			core.compileNewKernel(cf);
			needCLFractal = false;
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
		if (cf.buildKernel)
		{
			std::cout << "Need a new kernel, compiling it now.\n - requesting new texture\n - requesting new image\n";
			core.compileNewKernel(cf);
			cf.status.runKernel = true;
		}
		static paramCollector params_old = cf.params;
		static clFractalImage img_settings_old = cf.image;
		formulaSettingsWindow(cf);
		imageSettingsWindow(cf, textureColors);
		flameRenderSettingsWindow(cf);
		infoWindow(cf);
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
		if (!cf.running() and !cf.status.done)
		{
			if (cf.image.current_sample_count == cf.image.next_update_sample_count)
			{
				cf.image.next_update_sample_count = 2* cf.image.next_update_sample_count;
				cf.image.next_update_sample_count = cf.image.next_update_sample_count > cf.image.target_sample_count ? cf.image.target_sample_count : cf.image.next_update_sample_count;
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
				draggingCenter = true;
				dxCenter = ImGui::GetMouseDragDelta(0).x;
				dyCenter = ImGui::GetMouseDragDelta(0).y;
				offsetImageInBox(textureColors, vec_img_f_offset, ImGui::GetMouseDragDelta(0).x, ImGui::GetMouseDragDelta(0).y, cf.image.size.x, cf.image.size.y);
				refreshTexture(textureID, cf.image.size.x, cf.image.size.y, vec_img_f_offset);
			}
			else if (io.KeyShift)
			{
				imgBlocked = true;
				draggingZoom = true;
				dragZoomFactor = std::exp(-(float)ImGui::GetMouseDragDelta(0).y / 100.f);
				dragStartX = (int)((io.MousePos.x - coursorPos.x - ImGui::GetMouseDragDelta(0).x) / cf.image.zoom);
				dragStartY = (int)((io.MousePos.y - coursorPos.y - ImGui::GetMouseDragDelta(0).y) / cf.image.zoom);
				zoomImageInBox(textureColors, vec_img_f_offset, dragZoomFactor, dragStartX, dragStartY, cf.image.size.x, cf.image.size.y);
				refreshTexture(textureID, cf.image.size.x, cf.image.size.y, vec_img_f_offset);
			}
		}
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && draggingCenter)
		{
			draggingCenter = false;
			Complex<float> centerOffset = get_complex_offset(dxCenter, dyCenter, cf);
			cf.image.complexSubplane.x += centerOffset.x;
			cf.image.complexSubplane.y += centerOffset.y;
			textureColors = vec_img_f_offset;
			imgBlocked = false;
		}
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && draggingZoom)
		{
			draggingZoom = false;
			// calculate coordinate of pixel in the middle of the displayed image
			int imgDisplayCenterX = dragStartX + (cf.image.size.x / 2 - dragStartX) / dragZoomFactor;
			int imgDisplayCenterY = dragStartY + (cf.image.size.y / 2 - dragStartY) / dragZoomFactor;
			Complex new_center = get_complex_coord(imgDisplayCenterX, cf.image.size.y - imgDisplayCenterY, cf);
			cf.image.complexSubplane.x = new_center.x;
			cf.image.complexSubplane.y = new_center.y;
			cf.image.zoom *= dragZoomFactor;
			textureColors = vec_img_f_offset;
			imgBlocked = false;
		}
		ImGui::End();
		static char mainViewStr[] = "Main View";
		ImGui::Begin(mainViewStr, nullptr, ImGuiWindowFlags_HorizontalScrollbar);
		ImGui::Image((void*)(intptr_t)textureID, ImVec2(
			cf.image.size.x, cf.image.size.y)); // , texturesize);
		ImGui::End();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};