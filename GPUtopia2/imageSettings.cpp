#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#include "imageSettings.h"
#undef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

void save_to_png(const std::vector<color>& image_data, int imgWidth, const int imgHeight, char fnam[256])
{
	// write to file
	std::vector<uint8_t> image_sRGB(3 * imgWidth * imgHeight);
	float fMax = 0.f;
	uint32_t iMax = 0.f;
	for (unsigned int i = 0; i < 3 * imgWidth * imgHeight; i += 3) {
		color col = image_data[i / 3];
		image_sRGB[i] = uint8_t(col.r * 255);
		image_sRGB[i + 1] = uint8_t(col.g * 255);
		image_sRGB[i + 2] = uint8_t(col.b * 255);
		if (col.r > fMax) fMax = col.r;
		if (col.g > fMax) fMax = col.r;
		if (col.b > fMax) fMax = col.r;
		if (image_sRGB[i] > iMax) iMax = image_sRGB[i];
		if (image_sRGB[i + 1] > iMax) iMax = image_sRGB[i + 1];
		if (image_sRGB[i + 2] > iMax) iMax = image_sRGB[i + 2];
	}
	std::cout << "fMax = " << fMax << std::endl;
	std::cout << "iMax = " << iMax << std::endl;
	stbi_write_png(fnam, imgWidth, imgHeight, 3, &image_sRGB[0], imgWidth * 3);
}


void imageSettingsWindow(clFractal& cf, std::vector<color>& img)
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
	static std::string outName = "out.png";
	ImGui::InputText("File Name:", (char*)outName.c_str(), 256);
	if (ImGui::Button("Save"))
	{
		save_to_png(img, cf.image.size.x, cf.image.size.y, (char*)outName.c_str());
	}
	ImGui::End();
}