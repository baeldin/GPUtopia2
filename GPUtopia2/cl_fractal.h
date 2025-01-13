#pragma once
#include <map>

#include "gradient.h"
#include "sampling.h"

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
	cl_int2 size = { 800, 600 };
	float aspectRatio = (float)size.x / (float)size.y;
	cl_float4 complexSubplane = { 0.f, 0.f, 4.f, 4.f / aspectRatio };
	int targetQuality = 3;
	uint32_t currentQuality = 0;
	float zoom = 1.f;
	uint32_t current_sample_count = 0;
	uint32_t target_sample_count = 1;
	uint32_t next_update_sample_count = 1;
	clFractalImage() {
		float aspectRatio = (float)size.x / (float)size.y;
	}
	void updateComplexSubplane();
	void resetStatus() {
		this->currentQuality = 0;
		this->current_sample_count = 0;
		this->next_update_sample_count = 1;
		this->target_sample_count = fibonacci_number(this->targetQuality);
	}
};

inline const bool operator==(const clFractalImage& lhs, const clFractalImage& rhs)
{
	return(
		lhs.size == rhs.size &&
		lhs.aspectRatio == rhs.aspectRatio &&
		lhs.complexSubplane == rhs.complexSubplane &&
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

// Fractal class that holds parameters, names of the code fragmens, and the full
// CL code of the fractal + coloring
class clFractal
{
public:
	paramCollector params;
	clFractalImage image;
	Gradient gradient;
	std::string fractalCLFragmentFile = "clFragments/fractalFormulas/mandelbrot.cl";
	std::string coloringCLFragmentFile = "clFragments/coloringAlgorithms/by_iteration.cl";
	std::string fullCLcode = "";
	std::vector<int> imgIntRData;
	std::vector<int> imgIntGData;
	std::vector<int> imgIntBData;
	std::vector<int> imgIntAData;
	std::vector<color> imgData;
	bool vomit = false;
	int mode = 0;
	bool buildKernel = false;
	int maxIter = 100;
	float bailout = 4.f;
	// brightness, gamma, vibrancy
	cl_float4 flameRenderSettings = { 4.f, 2.f, 1.f, 0.f };
	cl_int flamePointSelection = 0;
	clFractalStatus status;
	clFractal() : gradient(Gradient::Gradient(4)) {}
	void makeCLCode();
	void setFractalCLFragmentFile(const char* fil) {
		fractalCLFragmentFile = std::string(fil);
	}
	void setColoringCLFragmentFile(const char* fil) {
		coloringCLFragmentFile = std::string(fil);
	}
	bool running() {
		return this->status.kernelRunning or this->status.imgKernelRunning;
	}

};

inline const bool operator==(const clFractal& lhs, const clFractal& rhs)
{
	return (
		lhs.params == rhs.params &&
		lhs.image == rhs.image &&
		lhs.buildKernel == rhs.buildKernel &&
		lhs.fractalCLFragmentFile == rhs.fractalCLFragmentFile &&
		lhs.coloringCLFragmentFile == rhs.coloringCLFragmentFile &&
		lhs.fullCLcode == rhs.fullCLcode &&
		lhs.maxIter == rhs.maxIter &&
		lhs.bailout == rhs.bailout &&
		lhs.gradient == rhs.gradient &&
		lhs.mode == rhs.mode &&
		// lhs.flameRenderSettings == rhs.flameRenderSettings &&
		lhs.flamePointSelection == rhs.flamePointSelection &&
		lhs.vomit == rhs.vomit);
}

inline const bool operator!=(const clFractal& lhs, const clFractal& rhs)
{
	return !(lhs == rhs);
}

