#include "fileOperations.h"

void newCLF(clFractal& cf, clCore& cc)
{
	cf = clFractal();
	cf.makeCLCode(NEW_FILES);
	cf.image.size.x = 1280;
	cf.image.size.y = 720;
	cf.image.resetStatus();
	cf.image.updateComplexSubplane();
	cc.resetCore();
	cc.compileFractalKernel(cf.fullCLcode);
}

void openCLF(clFractal& cf, clCore& cc)
{
	std::string path;
	bool success = false;

	openFileDialog(path, success, L"clf", L"GPUtopia Fractals (*.clf)");
	if (success)
	{
		std::ifstream inFile(path);
		std:cout << path << "\n";
		std::string jsonStr;
		inFile >> jsonStr;
		json back_json = json::parse(jsonStr);
		auto cf_inC = back_json.get<clFractalContainer>();
		clFractal cf_in(cf_inC);
		cf_in.resetCLFragmentQueue();
		cf_in.makeCLCode(SAME_FILES);
		cc.resetCore();
		cc.compileFractalKernel(cf_in.fullCLcode);
		cf = cf_in;
	}
}

void saveCLF(clFractal& cf)
{
	bool success = false;
	std::string path;
	saveFileDialog(path, success, L"clf", L"GPUtopia Fractals (*.clf)");
	if (success)
	{
		json json = cf.toExport();
		std::ofstream outFile(path);
		outFile << json.dump(4);
		outFile.close();
	}
}

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

void savePNG(const std::vector<color>& image_data, const size& size)
{
	bool success = false;
	std::string fileName;
	saveFileDialog(fileName, success, L"png", L"Portable Network Graphics (*.png)");
	if (success)
	{
		std::vector<uint8_t> image_sRGB(3 * size.x * size.y);
		float fMax = 0.f;
		uint32_t iMax = 0;
		for (unsigned int i = 0; i < 3 * size.x * size.y; i += 3) {
			color col = image_data[i / 3];
			image_sRGB[i] = uint8_t(col.r * 255);
			image_sRGB[i + 1] = uint8_t(col.g * 255);
			image_sRGB[i + 2] = uint8_t(col.b * 255);
			if (col.r > fMax) fMax = col.r;
			if (col.g > fMax) fMax = col.g;
			if (col.b > fMax) fMax = col.b;
			if (image_sRGB[i] > iMax) iMax = image_sRGB[i];
			if (image_sRGB[i + 1] > iMax) iMax = image_sRGB[i + 1];
			if (image_sRGB[i + 2] > iMax) iMax = image_sRGB[i + 2];
		}
		stbi_write_png((char*)fileName.c_str(), size.x, size.y, 3, &image_sRGB[0], size.x * 3);
	}
}
