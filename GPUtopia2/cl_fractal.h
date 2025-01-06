#pragma once
#include <map>

#include "gradient.h"
#include "sampling.h"

//const bool operator==(
//	const std::map<std::string, std::pair<cl_float2, int>>&lhs,
//	const std::map<std::string, std::pair<cl_float2, int>>&rhs) {
//	bool ret = true;
//	ret = (lhs.size() == rhs.size()) ? false : true;
//	for (auto [key, val] : lhs)
//	{
//		std::pair<cl_float2, int> val_rhs = rhs.find(key);
//
//	}
//}

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

//#ifndef HELLO
//#define HELLO
inline const bool operator==(const parameterMaps& lhs, const parameterMaps& rhs)
{
	return (
		lhs.integerParameters == rhs.integerParameters &&
		lhs.boolParameters == rhs.boolParameters &&
		lhs.floatParameters == rhs.floatParameters &&
		lhs.float2Parameters == rhs.float2Parameters);
}
//#endif
//const bool operator!=(const parameterMaps& lhs, const parameterMaps& rhs) {	return !(lhs == rhs); }

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
	int quality = 1;
	float zoom = 1.f;
	clFractalImage() {
		float aspectRatio = (float)size.x / (float)size.y;
	}
	void updateComplexSubplane();
};

inline const bool operator==(const clFractalImage& lhs, const clFractalImage& rhs)
{
	return(
		lhs.size == rhs.size &&
		lhs.aspectRatio == rhs.aspectRatio &&
		lhs.complexSubplane == rhs.complexSubplane &&
		lhs.quality == rhs.quality &&
		lhs.zoom == rhs.zoom);
}

inline const bool operator!=(const clFractalImage& lhs, const clFractalImage& rhs)
{
	return !(lhs == rhs);
}

// Fractal class that holds parameters, names of the code fragmens, and the full
// CL code of the fractal + coloring
class clFractal
{
public:
	paramCollector params;
	clFractalImage image;
	Gradient gradient;
	std::string fractalCLFragmentFile;
	std::string coloringCLFragmentFile;
	std::string fullCLcode = "";
	std::vector<int> imgIntRData;
	std::vector<int> imgIntGData;
	std::vector<int> imgIntBData;
	std::vector<int> imgIntAData;
	std::vector<color> imgData;
	bool vomit = false;
	int mode = 0;
	bool rebuildKernel = false;
	int maxIter = 100;
	float bailout = 4.f;
	// brightness, gamma, vibrancy, UNUSED
	cl_float4 flameRenderSettings = { 4.f, 2.f, 1.f, 0.f };
	cl_int flamePointSelection = 0;
	clFractal() : gradient(Gradient::Gradient(4)) {}
	void makeCLCode();
	void setFractalCLFragmentFile(const char* fil) {
		fractalCLFragmentFile = std::string(fil);
	}
	void setColoringCLFragmentFile(const char* fil) {
		coloringCLFragmentFile = std::string(fil);
	}
};

inline const bool operator==(const clFractal& lhs, const clFractal& rhs)
{
	return (
		lhs.params == rhs.params &&
		lhs.image == rhs.image &&
		lhs.rebuildKernel == rhs.rebuildKernel &&
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

