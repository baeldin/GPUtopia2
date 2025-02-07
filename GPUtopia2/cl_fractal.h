#pragma once
#include <map>
#include <cmath>
#include <fstream>
#include <iostream>
#include <mutex>

#include "json.hpp"
using json = nlohmann::json;

#include "gradient.h"
#include "sampling.h"
#include "complex_number.h"

#define NEW_FILES true
#define SAME_FILES false

inline const bool operator==(const cl_int2& lhs, const cl_int2& rhs)
{
	return
		(lhs.x == rhs.x && lhs.y == rhs.y);
}

inline const bool operator==(const cl_float2& lhs, const cl_float2& rhs)
{
	return
		(lhs.x == rhs.x && lhs.y == rhs.y);
}

inline const bool operator==(const cl_float4& lhs, const cl_float4& rhs)
{
	return
		(lhs.x == rhs.x && lhs.y == rhs.y &&
			lhs.z == rhs.z && lhs.w == rhs.w);
}

// flexible struct that holds parameters in a way that they can be used to make UI elements too
struct parameterMaps
{
	std::map<std::string, std::pair<int, int>> integerParameters;
	std::map<std::string, std::pair<bool, int>> boolParameters;
	std::map<std::string, std::pair<float, int>> floatParameters;
	std::map<std::string, std::pair<cl_float2, int>> float2Parameters;
};


inline const bool operator==(const parameterMaps& lhs, const parameterMaps& rhs)
{
	return (
		lhs.integerParameters == rhs.integerParameters &&
		lhs.boolParameters == rhs.boolParameters &&
		lhs.floatParameters == rhs.floatParameters &&
		lhs.float2Parameters == rhs.float2Parameters);
}

struct paramCollector
{
	parameterMaps fractalParameterMaps;
	parameterMaps coloringParameterMaps;
};

inline const bool operator==(const paramCollector& lhs, const paramCollector& rhs)
{
	return (
		lhs.fractalParameterMaps == rhs.fractalParameterMaps &&
		lhs.coloringParameterMaps == rhs.coloringParameterMaps);
}

// settings for the image generated from the fractal, including default values
struct clFractalImage
{
	cl_int2 size = { 1280, 720 };
	double aspectRatio = (double)size.x / (double)size.y;
	Complex<double> center = Complex<double>(0., 0.);
	Complex<double> span = Complex<double>(4., 4. / aspectRatio);
	// cl_float4 complexSubplane = { 0.f, 0.f, 4.f, 4.f / aspectRatio };
	int targetQuality = 3;
	uint32_t currentQuality = 0;
	double zoom = 1.;
	float angle = 0.f;
	uint32_t current_sample_count = 0;
	uint32_t target_sample_count = 1;
	uint32_t next_update_sample_count = 1;
	clFractalImage() {
		aspectRatio = (double)size.x / (double)size.y;
	}
	void updateComplexSubplane();
	void resetStatus() {
		this->currentQuality = 0;
		this->current_sample_count = 0;
		this->next_update_sample_count = 1;
		this->target_sample_count = fibonacci_number(this->targetQuality);
	}
	Complex<double> rotation() const { return Complex<double>(std::cos(angle / 180. * CL_M_PI), std::sin(angle / 180. * CL_M_PI)); }
	// cl_float2 complex2image(Complex<float> z);
	Complex<double> image2complex(const cl_double2 xy) const;
	Complex<double> image2complex(const cl_int2 xy_int) const
	{
		const cl_double2 xy = { xy_int.x, xy_int.y };
		return image2complex(xy);
	}
	Complex<double> image2complex(const float x, const float y) const
	{
		const cl_double2 xy = { x, y };
		return image2complex(xy);
	}
	Complex<double> image2complex(const int x, const int y) const
	{
		const cl_double2 xy = { x, y };
		return image2complex(xy);
	}
};


inline const bool operator==(const clFractalImage& lhs, const clFractalImage& rhs)
{
	return(
		lhs.size == rhs.size &&
		lhs.aspectRatio == rhs.aspectRatio &&
		// lhs.complexSubplane == rhs.complexSubplane &&
		lhs.center == rhs.center &&
		lhs.angle == rhs.angle &&
		lhs.targetQuality == rhs.targetQuality &&
		lhs.zoom == rhs.zoom);
}

inline const bool operator!=(const clFractalImage& lhs, const clFractalImage& rhs)
{
	return !(lhs == rhs);
}

struct clFractalStatus
{
	bool kernelRunning = false;
	bool imgKernelRunning = false;
	bool runKernel = false;
	bool runImgKernel = false;
	bool updateImage = false;
	bool done = false;
};

class clFractal;

struct clFractalMinimal
{
	double centerX;
	double centerY;
	double spanX;
	double spanY;
	double zoom;
	double angle;
	int sizeX;
	int sizeY;
	int quality;
	float brightness;
	float gamma;
	float vibrancy;
	int maxIter;
	double bailout;
	int mode;
	int pointSelection;
	bool useDouble;
	std::map<std::string, std::pair<int, int>> fractalIntParameters;
	std::map<std::string, std::pair<float, int>> fractalFloatParameters;
	std::map<std::string, std::pair<bool, int>> fractalBoolParameters;
	std::map<std::string, std::pair<int, int>> coloringIntParameters;
	std::map<std::string, std::pair<float, int>> coloringFloatParameters;
	std::map<std::string, std::pair<bool, int>> coloringBoolParameters;
	std::vector<std::pair<int, int>> gradientColors;
	std::vector<int> gradientFillOrder;
	std::string fractalCLFragmentFile;
	std::string coloringCLFragmentFile;
	clFractalMinimal() {}
	clFractalMinimal(const clFractal* cf);
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(clFractalMinimal,
		centerX, centerY, zoom, angle, sizeX, sizeY, quality,
		brightness, gamma, vibrancy, maxIter, bailout, mode, pointSelection, useDouble,
		fractalIntParameters, fractalFloatParameters, fractalBoolParameters,
		coloringIntParameters, coloringFloatParameters, coloringBoolParameters,
		gradientColors, gradientFillOrder,
		fractalCLFragmentFile, coloringCLFragmentFile);
};

inline std::mutex timingMutex;

// Fractal class that holds parameters, names of the code fragmens, and the full
// CL code of the fractal + coloring
class clFractal
{
public:
	paramCollector params;
	clFractalImage image;
	Gradient gradient;
	// TODO: think of a better solution to avoid collisions from the two places that can set
	// a new file: load function vs. undo/redo :-(
	std::string fractalCLFragmentFile = "clFragments/fractalFormulas/mandelbrot.cl";
	std::string fractalCLFragmentFileUi = "clFragments/fractalFormulas/mandelbrot.cl";
	std::string fractalCLFragmentFileHist = "clFragments/fractalFormulas/mandelbrot.cl";
	std::string coloringCLFragmentFile = "clFragments/coloringAlgorithms/by_iteration.cl";
	std::string coloringCLFragmentFileUi = "clFragments/coloringAlgorithms/by_iteration.cl";
	std::string coloringCLFragmentFileHist = "clFragments/coloringAlgorithms/by_iteration.cl";
	std::string fullCLcode = "";
	std::vector<int> imgIntRGBAData;
	std::vector<color> imgData;
	std::vector<float> timings;
	bool stop = false;
	bool useDouble = false;
	bool vomit = false;
	int mode = 0;
	bool buildKernel = false;
	int maxIter = 100;
	float bailout = 4.f;
	int samples_per_kernel_run = 1;
	// brightness, gamma, vibrancy
	cl_float4 flameRenderSettings = { 4.f, 2.f, 1.f, 0.f };
	cl_int flamePointSelection = 0;
	cl_int flameWarmup = 0;
	clFractalStatus status;
	uint32_t verbosity = 0;
	clFractal() : gradient() {}
	clFractal(const clFractalMinimal& cfm);
	void makeCLCode(const bool sameFiles = NEW_FILES);
	void setFractalCLFragmentFile(const char* fil) {
		fractalCLFragmentFile = std::string(fil);
	}
	void setColoringCLFragmentFile(const char* fil) {
		coloringCLFragmentFile = std::string(fil);
	}
	bool running() {
		return this->status.kernelRunning or this->status.imgKernelRunning;
	}
	bool newFractalCLFragmentQueued() const
	{
		return (
			this->fractalCLFragmentFile != this->fractalCLFragmentFileHist ||
			this->fractalCLFragmentFile != this->fractalCLFragmentFileUi);
	}
	bool newColoringCLFragmentQueued() const
	{
		return (
			this->coloringCLFragmentFile != this->coloringCLFragmentFileHist ||
			this->coloringCLFragmentFile != this->coloringCLFragmentFileUi);
	}
	std::string getQueuedFractalCLFile() const
	{
		if (fractalCLFragmentFile != fractalCLFragmentFileUi)
			return fractalCLFragmentFileUi;
		if (fractalCLFragmentFile != fractalCLFragmentFileHist)
			return fractalCLFragmentFileHist;
		else // this should never happen
			return std::string("void");
	}
	std::string getQueuedColoringCLFile() const
	{
		if (coloringCLFragmentFile != coloringCLFragmentFileUi)
			return coloringCLFragmentFileUi;
		if (coloringCLFragmentFile != coloringCLFragmentFileHist)
			return coloringCLFragmentFileHist;
		else // this should never happen
			return std::string("void");
	}
	void popFractalCLFragmentQueue()
	{
		const std::string newFractalCLFile = this->getQueuedFractalCLFile();
		this->fractalCLFragmentFile = newFractalCLFile;
		this->fractalCLFragmentFileHist = newFractalCLFile;
		this->fractalCLFragmentFileUi = newFractalCLFile;
	}
	void popColoringCLFragmentQueue()
	{
		const std::string newColoringCLFile = this->getQueuedColoringCLFile();
		this->coloringCLFragmentFile = newColoringCLFile;
		this->coloringCLFragmentFileHist = newColoringCLFile;
		this->coloringCLFragmentFileUi = newColoringCLFile;
	}
	bool newCLFragmentQueued() { return (newFractalCLFragmentQueued() || newColoringCLFragmentQueued()); }
	clFractalMinimal toExport() { return clFractalMinimal(this); }
};

inline const bool operator==(const clFractal& lhs, const clFractal& rhs)
{
	return (
		lhs.params == rhs.params &&
		lhs.image == rhs.image &&
		// lhs.buildKernel == rhs.buildKernel &&
		lhs.fractalCLFragmentFile == rhs.fractalCLFragmentFile &&
		lhs.fractalCLFragmentFileHist == rhs.fractalCLFragmentFileHist &&
		lhs.fractalCLFragmentFileUi == rhs.fractalCLFragmentFileUi &&
		lhs.coloringCLFragmentFile == rhs.coloringCLFragmentFile &&
		lhs.coloringCLFragmentFileHist == rhs.coloringCLFragmentFileHist &&
		lhs.coloringCLFragmentFileUi == rhs.coloringCLFragmentFileUi &&
		// lhs.fullCLcode == rhs.fullCLcode &&
		lhs.maxIter == rhs.maxIter &&
		lhs.bailout == rhs.bailout &&
		lhs.useDouble == rhs.useDouble &&
		//lhs.gradient == rhs.gradient &&
		lhs.mode == rhs.mode &&
		// lhs.flameRenderSettings == rhs.flameRenderSettings &&
		lhs.flamePointSelection == rhs.flamePointSelection &&
		lhs.vomit == rhs.vomit);
}

inline const bool operator!=(const clFractal& lhs, const clFractal& rhs)
{
	return !(lhs == rhs);
}

