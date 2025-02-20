#pragma once
#include <map>
#include <cmath>
#include <fstream>
#include <iostream>
#include <mutex>

#include "json.hpp"
using json = nlohmann::ordered_json;

#include "gradient.h"
#include "sampling.h"
#include "complex_number.h"

#define NEW_FILES true
#define SAME_FILES false

typedef std::map<std::string, std::pair<int, int>> parameterMapInt;
typedef std::map<std::string, std::pair<bool, int>> parameterMapBool;
typedef std::map<std::string, std::pair<double, int>> parameterMapReal;
typedef std::map<std::string, std::pair<Complex<double>, int>> parameterMapComplex;

typedef std::map<std::string, std::pair<float, int>> parameterMapFloat;
typedef std::map<std::string, std::pair<Complex<float>, int>> parameterMapComplexFloat;

// support enum type parameter that is displayed as combo in the Ui
struct enumParameter
{
	int value;
	std::vector<std::string> labels;
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(enumParameter, value, labels);
};

inline const bool operator==(const enumParameter& lhs, const enumParameter& rhs)
{
	return (lhs.value == rhs.value);
}

typedef std::map<std::string, std::pair<enumParameter, int>> parameterMapEnum;

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
	parameterMapInt integerParameters;
	parameterMapBool boolParameters;
	parameterMapReal realParameters;
	parameterMapComplex complexParameters;	
	parameterMapEnum enumParameters;
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(parameterMaps, integerParameters, boolParameters,
		realParameters, complexParameters, enumParameters);
};


inline const bool operator==(const parameterMaps& lhs, const parameterMaps& rhs)
{
	return (
		lhs.integerParameters == rhs.integerParameters &&
		lhs.boolParameters == rhs.boolParameters &&
		lhs.realParameters == rhs.realParameters &&
		lhs.complexParameters == rhs.complexParameters &&
		lhs.enumParameters == rhs.enumParameters);
}

struct paramCollector
{
	parameterMaps fractalParameterMaps;
	parameterMaps insideColoringParameterMaps;
	parameterMaps outsideColoringParameterMaps;
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(paramCollector, fractalParameterMaps, 
		outsideColoringParameterMaps, insideColoringParameterMaps);
};

inline const bool operator==(const paramCollector& lhs, const paramCollector& rhs)
{
	return (
		lhs.fractalParameterMaps == rhs.fractalParameterMaps &&
		lhs.insideColoringParameterMaps == rhs.insideColoringParameterMaps &&
		lhs.outsideColoringParameterMaps == rhs.outsideColoringParameterMaps);
}

struct size
{
	int x;
	int y;
	size() : x(1280), y(720) {}
	size(const float x_, const float y_) : x(x_), y(y_) {}
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(size, x, y);
};

inline const bool operator==(const size& lhs, const size& rhs)
{
	return (lhs.x == rhs.x && lhs.y == rhs.y);
}

inline const bool operator!=(const size& lhs, const size& rhs) { return !(lhs == rhs); }

// settings for the image generated from the fractal, including default values
struct clFractalImage
{
	size size = { 1280, 720 };
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
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(clFractalImage, center, zoom, angle, size, targetQuality, span);
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
	bool filesOK = false;
};

class clFractal;

inline std::mutex timingMutex;

struct flameRenderSettings
{
	float brightness = 4.f;
	float gamma = 1.5f;
	float vibrancy = 1.f;
	flameRenderSettings() {}
	flameRenderSettings(const float b_, const float g_, const float v_) : brightness(b_), gamma(g_), vibrancy(v_) {}
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(flameRenderSettings, brightness, gamma, vibrancy);
};

inline const bool operator==(const flameRenderSettings& lhs, const flameRenderSettings& rhs)
{
	return (lhs.brightness == rhs.brightness &&
		lhs.gamma == rhs.gamma &&
		lhs.vibrancy == rhs.vibrancy);
}

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
	std::string outsideColoringCLFragmentFile = "clFragments/coloringAlgorithms/by_iteration.cl";
	std::string outsideColoringCLFragmentFileUi = "clFragments/coloringAlgorithms/by_iteration.cl";
	std::string outsideColoringCLFragmentFileHist = "clFragments/coloringAlgorithms/by_iteration.cl";
	std::string insideColoringCLFragmentFile = "clFragments/coloringAlgorithms/gaussian_integer.cl";
	std::string insideColoringCLFragmentFileUi = "clFragments/coloringAlgorithms/gaussian_integer.cl";
	std::string insideColoringCLFragmentFileHist = "clFragments/coloringAlgorithms/gaussian_integer.cl";
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
	flameRenderSettings frs = { 4.f, 2.f, 1.f };
	cl_int flamePointSelection = 0;
	cl_int flameWarmup = 0;
	clFractalStatus status;
	uint32_t verbosity = 0;
	clFractal() : gradient() {}
	bool makeCLCode(const bool sameFiles = NEW_FILES);
	bool running() {
		return this->status.kernelRunning or this->status.imgKernelRunning;
	}
	bool newFractalCLFragmentQueued() const
	{
		return (
			this->fractalCLFragmentFile != this->fractalCLFragmentFileHist ||
			this->fractalCLFragmentFile != this->fractalCLFragmentFileUi);
	}
	bool newInsideColoringCLFragmentQueued() const
	{
		return (
			this->insideColoringCLFragmentFile != this->insideColoringCLFragmentFileHist ||
			this->insideColoringCLFragmentFile != this->insideColoringCLFragmentFileUi);
	}
	bool newOutsideColoringCLFragmentQueued() const
	{
		return (
			this->outsideColoringCLFragmentFile != this->outsideColoringCLFragmentFileHist ||
			this->outsideColoringCLFragmentFile != this->outsideColoringCLFragmentFileUi);
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
	std::string getQueuedOutsideColoringCLFile() const
	{
		if (outsideColoringCLFragmentFile != outsideColoringCLFragmentFileUi)
			return outsideColoringCLFragmentFileUi;
		if (outsideColoringCLFragmentFile != outsideColoringCLFragmentFileHist)
			return outsideColoringCLFragmentFileHist;
		else // this should never happen
			return std::string("void");
	}
	std::string getQueuedInsideColoringCLFile() const
	{
		if (insideColoringCLFragmentFile != insideColoringCLFragmentFileUi)
			return insideColoringCLFragmentFileUi;
		if (insideColoringCLFragmentFile != insideColoringCLFragmentFileHist)
			return insideColoringCLFragmentFileHist;
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
	void popOutsideColoringCLFragmentQueue()
	{
		const std::string newOutsideColoringCLFile = this->getQueuedOutsideColoringCLFile();
		this->outsideColoringCLFragmentFile = newOutsideColoringCLFile;
		this->outsideColoringCLFragmentFileHist = newOutsideColoringCLFile;
		this->outsideColoringCLFragmentFileUi = newOutsideColoringCLFile;
	}
	void popInsideColoringCLFragmentQueue()
	{
		const std::string newInsideColoringCLFile = this->getQueuedInsideColoringCLFile();
		this->insideColoringCLFragmentFile = newInsideColoringCLFile;
		this->insideColoringCLFragmentFileHist = newInsideColoringCLFile;
		this->insideColoringCLFragmentFileUi = newInsideColoringCLFile;
	}
	bool newCLFragmentQueued() { return (
		newFractalCLFragmentQueued() ||
		newOutsideColoringCLFragmentQueued() ||
		newInsideColoringCLFragmentQueued()); }
	bool clFragmentExists(const std::string& fileName) const
	{
		std::ifstream f(fileName.c_str());
		return f.good();
	}
	bool pathOK(std::string& filePath, const std::string& fragmentFolder) 
	{
		std::filesystem::path p(filePath);
		std::string defaultRelativePath = "clFragments\\" + fragmentFolder + "\\" + p.filename().string();
		if (clFragmentExists(filePath))
			return true;
		else if (clFragmentExists(defaultRelativePath))
		{
			filePath = defaultRelativePath;
			return true;
		}
		else
			return false;
	}
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(clFractal,
		image, frs, maxIter, bailout, mode, flamePointSelection, useDouble,
		params, gradient, fractalCLFragmentFile, insideColoringCLFragmentFile, outsideColoringCLFragmentFile); 
};

inline const bool operator==(const clFractal& lhs, const clFractal& rhs)
{
	//std::cout << "lhs.params == rhs.params " << (lhs.params == rhs.params) << "\n";
	//std::cout << "lhs.image == rhs.image " << (lhs.image == rhs.image) << "\n";
	//std::cout << "lhs.fractalCLFragmentFile == rhs.fractalCLFragmentFile " << (lhs.fractalCLFragmentFile == rhs.fractalCLFragmentFile) << "\n";
	//std::cout << "lhs.fractalCLFragmentFileHist == rhs.fractalCLFragmentFileHist " << (lhs.fractalCLFragmentFileHist == rhs.fractalCLFragmentFileHist) << "\n";
	//std::cout << "lhs.fractalCLFragmentFileUi == rhs.fractalCLFragmentFileUi " << (lhs.fractalCLFragmentFileUi == rhs.fractalCLFragmentFileUi) << "\n";
	//std::cout << "lhs.coloringCLFragmentFile == rhs.coloringCLFragmentFile " << (lhs.coloringCLFragmentFile == rhs.coloringCLFragmentFile) << "\n";
	//std::cout << "lhs.coloringCLFragmentFileHist == rhs.coloringCLFragmentFileHist " << (lhs.coloringCLFragmentFileHist == rhs.coloringCLFragmentFileHist) << "\n";
	//std::cout << "lhs.coloringCLFragmentFileUi == rhs.coloringCLFragmentFileUi " << (lhs.coloringCLFragmentFileUi == rhs.coloringCLFragmentFileUi) << "\n";
	//std::cout << "lhs.maxIter == rhs.maxIter " << (lhs.maxIter == rhs.maxIter) << "\n";
	//std::cout << "lhs.bailout == rhs.bailout " << (lhs.bailout == rhs.bailout) << "\n";
	//std::cout << "lhs.useDouble == rhs.useDouble " << (lhs.useDouble == rhs.useDouble) << "\n";
	//std::cout << "lhs.gradient == rhs.gradient " << (lhs.gradient == rhs.gradient) << "\n";
	//std::cout << "lhs.mode == rhs.mode " << (lhs.mode == rhs.mode) << "\n";
	//std::cout << "lhs.flamePointSelection == rhs.flamePointSelection " << (lhs.flamePointSelection == rhs.flamePointSelection) << "\n";
	//std::cout << "lhs.vomit == rhs.vomit " << (lhs.vomit == rhs.vomit) << "\n";
	return (
		lhs.params == rhs.params &&
		lhs.image == rhs.image &&
		// lhs.buildKernel == rhs.buildKernel &&
		lhs.fractalCLFragmentFile == rhs.fractalCLFragmentFile &&
		lhs.fractalCLFragmentFileHist == rhs.fractalCLFragmentFileHist &&
		lhs.fractalCLFragmentFileUi == rhs.fractalCLFragmentFileUi &&
		lhs.outsideColoringCLFragmentFile == rhs.outsideColoringCLFragmentFile &&
		lhs.outsideColoringCLFragmentFileHist == rhs.outsideColoringCLFragmentFileHist &&
		lhs.outsideColoringCLFragmentFileUi == rhs.outsideColoringCLFragmentFileUi &&
		lhs.insideColoringCLFragmentFile == rhs.insideColoringCLFragmentFile &&
		lhs.insideColoringCLFragmentFileHist == rhs.insideColoringCLFragmentFileHist &&
		lhs.insideColoringCLFragmentFileUi == rhs.insideColoringCLFragmentFileUi &&
		// lhs.fullCLcode == rhs.fullCLcode &&
		lhs.maxIter == rhs.maxIter &&
		lhs.bailout == rhs.bailout &&
		lhs.useDouble == rhs.useDouble &&
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

