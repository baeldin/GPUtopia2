#include "fractal_navigation.h"

void dragPan(clFractal& cf, fractalNavigationParameters& nav, ImGuiIO& io)
{
	nav.draggingCenter = true;
	nav.dragOffset.x = ImGui::GetMouseDragDelta(0).x;
	nav.dragOffset.y = ImGui::GetMouseDragDelta(0).y;
}

void dragZoom(clFractal& cf, fractalNavigationParameters& nav, ImGuiIO& io)
{
	nav.draggingZoom = true;
	nav.dragOffset.x = ImGui::GetMouseDragDelta(0).x;
	nav.dragOffset.y = ImGui::GetMouseDragDelta(0).y;
	nav.dragZoomFactor = std::exp(-(float)ImGui::GetMouseDragDelta(0).y / 100.f);
	nav.dragStart.x = (int)((io.MousePos.x - nav.coursorPos.x - nav.dragOffset.x));
	nav.dragStart.y = (int)((io.MousePos.y - nav.coursorPos.y - nav.dragOffset.y));
}

void offsetImageInBox(const std::vector<color>& image, std::vector<color>& offsetImage, 
	clFractal& cf, fractalNavigationParameters& nav)
{
	// move the pixels within the image to simulate moving out outside the frame, replace black
	// pixels with black.
	int maxIndex = cf.image.size.x * cf.image.size.y;
	std::fill(offsetImage.begin(), offsetImage.end(), 0.f);
	for (int y = 0; y < cf.image.size.y; y++)
	{
		for (int x = 0; x < cf.image.size.x; x++)
		{
			// check if pixelIndexNew is out of bounds:
			if (y + nav.dragOffset.y >= 0 && x + nav.dragOffset.x >= 0 && 
				y + nav.dragOffset.y < cf.image.size.y && x + nav.dragOffset.x < cf.image.size.x)
				//std::cout << x << " " << y << " " << dx << " " << dy << " " << pixelIndexOld << " " << pixelIndexNew << "\n";
			{
				const int pixelIndexOld = y * cf.image.size.x + x;
				const int pixelIndexNew = (y + nav.dragOffset.y) * cf.image.size.x + x + nav.dragOffset.x;
				offsetImage[pixelIndexNew] = image[pixelIndexOld];
			}
		}
	}
}

void zoomImageInBox(const std::vector<color>& image, std::vector<color>& offsetImage,
	clFractal& cf, fractalNavigationParameters& nav)
{
	/* zoom into the image using a pixel coordinate center and zoom factor */
	const int maxIndex = cf.image.size.x * cf.image.size.y;
	std::fill(offsetImage.begin(), offsetImage.end(), 0.f);
	for (int y = 0; y < cf.image.size.y; y++)
	{
		for (int x = 0; x < cf.image.size.x; x++)
		{
			// find out which old pixel should be mapped to the zoomed image
			const int oldX = nav.dragStart.x - (nav.dragStart.x - x) / nav.dragZoomFactor;
			const int oldY = nav.dragStart.y - (nav.dragStart.y - y) / nav.dragZoomFactor;
			if (oldY >= 0 && oldX >= 0 && oldY < cf.image.size.y && oldX < cf.image.size.x) // check inf inside:
			{
				const int pixelIndexOld = oldY * cf.image.size.x + oldX;
				const int pixelIndexNew = y * cf.image.size.x + x;
				offsetImage[pixelIndexNew] = image[pixelIndexOld];
			}
		}
	}
}

cl_float2 get_image_center_after_zoom(clFractal& cf, fractalNavigationParameters& nav)
{
	const cl_float2 oldCenter = { (float)cf.image.size.x / 2.f, (float)cf.image.size.y / 2.f };
	const cl_float2 dragCenterDist = { (float)nav.dragStart.x - oldCenter.x, (float)nav.dragStart.y - oldCenter.y };
	return oldCenter + dragCenterDist / nav.dragZoomFactor;
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
	
}
