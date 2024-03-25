#include "imageSettings.h"

void imageSettingsWindow(clFractal& cf)
{
	ImGui::Begin("Image");
	static float xmin = -5.f;
	static float xmax = 5.f;
	static float ymin = -5.f;
	static float ymax = 5.f;
	static float xCenter = cf.image.complexSubplane.x;
	static float yCenter = cf.image.complexSubplane.y;
	ImGui::InputInt("Iterations", &cf.maxIter, 1, 100);
	ImGui::InputFloat("Zoom", &cf.image.zoom, 0.1f, 10.f);
	static cl_float2 center = { cf.image.complexSubplane.x, cf.image.complexSubplane.y };
	ImGui::SliderFloat("Center Real", &cf.image.complexSubplane.x, xmin, xmax);
	ImGui::SliderFloat("Center Imag", &cf.image.complexSubplane.y, ymin, ymax);
	if (!ImGui::IsMouseDragging(ImGuiMouseButton_Left))
	{
		xmin = cf.image.complexSubplane.x - 2.f / cf.image.zoom;
		xmax = cf.image.complexSubplane.x + 2.f / cf.image.zoom;
		ymin = cf.image.complexSubplane.y - 2.f / cf.image.zoom / cf.image.aspectRatio;
		ymax = cf.image.complexSubplane.y + 2.f / cf.image.zoom / cf.image.aspectRatio;
	}
	ImGui::SliderInt("Render Quality", &cf.image.quality, 1, 16);
	ImGui::InputInt("Width", &cf.image.size.x);
	ImGui::InputInt("Height", &cf.image.size.y);
	ImGui::InputInt("Coloring mode", &cf.mode);
	ImGui::End();
}