
#include "imageSettings.h"

void imageSettingsWindow(clFractal& cf)
{
	ImGui::Begin("Image");
	static double xmin = -5.f;
	static double xmax = 5.f;
	static double ymin = -5.f;
	static double ymax = 5.f;
	// static float xCenter = cf.image.center.x;
	// static float yCenter = cf.image.center.y;
	ImGui::InputInt("Iterations", &cf.maxIter, 1, 100);
	ImGui::InputDouble("Zoom", &cf.image.zoom, 0.1f, 10.f);
	ImGui::InputFloat("Rotation", &cf.image.angle, 0.1f, 45.f);
	// static cl_float2 center = { cf.image.complexSubplane.x, cf.image.complexSubplane.y };
	ImGui::SliderScalar("Center Real", ImGuiDataType_Double , &cf.image.center.x, &xmin, &xmax);
	ImGui::SliderScalar("Center Imag", ImGuiDataType_Double , &cf.image.center.y, &ymin, &ymax);
	if (!ImGui::IsMouseDragging(ImGuiMouseButton_Left))
	{
		xmin = cf.image.center.x - 2.f / cf.image.zoom;
		xmax = cf.image.center.x + 2.f / cf.image.zoom;
		ymin = cf.image.center.y - 2.f / cf.image.zoom / cf.image.aspectRatio;
		ymax = cf.image.center.y + 2.f / cf.image.zoom / cf.image.aspectRatio;
	}
	ImGui::SliderInt("Target Quality", &cf.image.targetQuality, 1, 30);
	ImGui::InputInt("Width", &cf.image.size.x);
	ImGui::InputInt("Height", &cf.image.size.y);
	const char* tonemappingItems[] = { "Escape Time", "Flame" };
	ImGui::Combo("Tonemapping Mode", &cf.mode, tonemappingItems, IM_ARRAYSIZE(tonemappingItems));
	static std::string outName = "out.png";
	ImGui::InputText("File Name:", (char*)outName.c_str(), 256);
	ImGui::End();
}