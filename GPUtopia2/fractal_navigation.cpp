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

void dragRotate(clFractal& cf, fractalNavigationParameters& nav, ImGuiIO& io)
{
	nav.draggingRotate = true;
	nav.mouseStartToCenterX = nav.dragStart.x - cf.image.size.x / 2.f;
	nav.mouseStartToCenterY = nav.dragStart.y - cf.image.size.y / 2.f;
	nav.mouseToCenterX = nav.dragStart.x + ImGui::GetMouseDragDelta(0).x - cf.image.size.x / 2.f;
	nav.mouseToCenterY = nav.dragStart.y + ImGui::GetMouseDragDelta(0).y - cf.image.size.y / 2.f;
	nav.startAngle = -atan2(nav.mouseStartToCenterY, nav.mouseStartToCenterX);
	nav.dragAngle = atan2(nav.mouseToCenterY, nav.mouseToCenterX) + nav.startAngle;
	nav.dragRotation = Complex<double>(cos(nav.dragAngle), sin(nav.dragAngle));
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

void rotateImageInBox(const std::vector<color>& image, std::vector<color>& offsetImage,
	clFractal& cf, fractalNavigationParameters& nav)
{
	/* zoom into the image using a pixel coordinate center and zoom factor */
	int maxIndex = cf.image.size.x * cf.image.size.y;
	std::fill(offsetImage.begin(), offsetImage.end(), 0.f);
	const Complex<double> zRot = Complex<double>(std::cos(nav.dragAngle), std::sin(nav.dragAngle)).conj();
	for (int y = 0; y < cf.image.size.y; y++)
	{
		for (int x = 0; x < cf.image.size.x; x++)
		{
			// find out which old pixel should be mapped to the zoomed image
			// TODO: reverse lookup from new to old img to prevent black pixel glitch
			int pixelIndexNew = y * cf.image.size.x + x;
			const Complex<double> imgCenter = Complex<double>(cf.image.size.x / 2.f, cf.image.size.y / 2.f);
			Complex<double> pixelPosOld = (Complex<double>(x, y) - imgCenter) * zRot + imgCenter;
			int xOld = (int)pixelPosOld.x;
			int yOld = (int)pixelPosOld.y;
			if (xOld >= 0 && yOld >= 0 && xOld < cf.image.size.x && yOld < cf.image.size.y) // check inf inside:
			{
				int pixelIndexOld = yOld * cf.image.size.x + xOld;
				offsetImage[pixelIndexNew] = image[pixelIndexOld];
			}
		}
	}
}

cl_double2 imgCoordinateCenterAfterZoom(clFractal& cf, fractalNavigationParameters& nav)
{
	const cl_double2 ret = {
		nav.dragStart.x - (nav.dragStart.x - (double)cf.image.size.x / 2.) / nav.dragZoomFactor,
		nav.dragStart.y - (nav.dragStart.y - (double)cf.image.size.y / 2.) / nav.dragZoomFactor
	};
	std::cout << "RET = (" << ret.x << ", " << ret.y << ")\n";
	return ret;
}

Complex<double> get_complex_offset(const int dx, const int dy, const clFractal& cf)
{
	const double dxDouble = (double)dx * cf.image.zoom;
	const double dyDouble = (double)dy * cf.image.zoom;
	const double xRange = cf.image.span.x / cf.image.zoom;
	const double yRange = cf.image.span.y / cf.image.zoom;
	double xOffset = -dxDouble / cf.image.size.x * xRange;
	double yOffset = dyDouble / cf.image.size.y * yRange;
	return Complex<double>(xOffset, yOffset) * cf.image.rotation();
}