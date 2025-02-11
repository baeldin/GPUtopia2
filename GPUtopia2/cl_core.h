#pragma once

#define CL_USE_DEPRECATED_OPENCL_2_2_APIS

#include <iostream>
#include <vector>
#include <string>
     #include <map>
#include <typeinfo>
#include <chrono>
#include <CL/cl2.hpp>
#include <cmath>
#include <stdexcept>

// Include settings and types from other parts of the project.
#include "formulaSettings.h"
#include "color.h"

constexpr int WG_SIZE = 256; // Workgroup size

// Enum for fractal kernel argument indices.
// Note: You may need to update other parts of the code if these indices are referenced.
enum class FractalKernelArg : int {
    IMAGE_SIZE = 0,
    COMPLEX_SUBPLANE = 1,
    ROTATION = 2,
    SAMPLING_INFO = 3,
    ITERATIONS = 4,
    BAILOUT = 5,
    GRADIENT_LENGTH = 6,
    GRADIENT_COLORS = 7,
    IMG_INT_RGBA = 8,
    FLAME_POINT_SELECTION = 9,
    FLAME_WARMUP = 10,
    FRACTAL_MODE = 11,
    // Add any further indices here if needed.
};

struct err
{
    cl_int programError = CL_SUCCESS;
    cl_int compileError = CL_SUCCESS;
    cl_int kernelError = CL_SUCCESS;
    cl_int sum() const { return programError + compileError + kernelError; }
    void reset() {
        programError = CL_SUCCESS;
        compileError = CL_SUCCESS;
        kernelError = CL_SUCCESS;
    }
};

class clKernelContainer
{
public:
    cl::Kernel kernel;
    cl_int argumentCount;
    cl::Program program;
    bool need = true;
    std::string fullCode;
    std::string buildLog;
    err errors;
    clKernelContainer() {}
    ~clKernelContainer() {}
};

class clCore
{
public:
    // general stuff
    cl::Platform platform;
    cl::Device device;
    cl::Context context;
    cl::CommandQueue queue;
    // kernel containers for fractal and image kernels
    clKernelContainer fractalKernel;
    clKernelContainer imgKernel;
    // buffers
    cl::Buffer gradientBuffer;
    cl::Buffer imgIntRGBABuffer;
    cl::Buffer imgFloatBuffer;
    // other
    cl_int queueError = CL_SUCCESS;
    int currentRenderSize = 0;
    bool stop = false;

    clCore()
    {
        setContext();
        setQueue();
    }
    void setContext();
    void setQueue();
    // fractal kernel
    cl_int compileNewKernel(clKernelContainer& kc, const std::string& fullCLcode,
        const std::string kernelFunctionName);
    void compileFractalKernel(const std::string fullCLcode);
    void resetCore();
    // image kernel
    void compileImgKernel();

    // Error checking helper
    inline void checkError(cl_int err, const std::string& msg, int verbosity = 1) const {
        if (err != CL_SUCCESS)
            std::cerr << msg << " Error code: " << err << std::endl;
        else if (verbosity >= 1)
            std::cout << msg << " completed successfully.\n";
    }

    // Methods for setting arguments
    template <typename T>
    inline cl::Buffer setBufferKernelArg(cl::Kernel& currentKernel, int k, T* data, size_t size, cl_mem_flags mem_flag,
        const char* name, cl_int* err_out, const int verbosity = 99)
    {
        cl_int err = CL_SUCCESS;
        cl::Buffer buff(this->context, mem_flag, size, nullptr, &err);
        if (err != CL_SUCCESS)
            std::cerr << "Error " << err << " while creating buffer " << name << std::endl;
        else if (verbosity >= 1)
            std::cout << "Buffer " << name << " with type " << typeid(data).name() << " and size " << size << " created successfully.\n";

        err = this->queue.enqueueWriteBuffer(buff, CL_TRUE, 0, size, data, nullptr, nullptr);
        if (err != CL_SUCCESS)
            std::cerr << "Error " << err << " while enqueueing buffer " << name << std::endl;
        else if (verbosity >= 1)
            std::cout << "Buffer write for " << name << " with type " << typeid(data).name() << " and size " << size << " enqueued successfully.\n";

        err = currentKernel.setArg(k, buff);
        if (err != CL_SUCCESS)
            std::cerr << "Failed to set kernel argument. Error code: " << err << std::endl;
        else if (verbosity >= 1)
            std::cout << "Successfully set kernel argument " << name << " at index : " << k << " with buffer : " << &buff << std::endl;
        if (err_out) *err_out = err;
        return buff;
    }

    inline void setReusedBufferArgument(cl::Kernel& currentKernel, int k, cl::Buffer& buff, const char* name = "unnamed_reused_buffer_kernel_arg", const int verbosity = 99)
    {
        cl_int err = currentKernel.setArg(k, buff);
        if (err != CL_SUCCESS)
            std::cerr << "Failed to set reused buffer kernel argument. Error code: " << err << std::endl;
        else if (verbosity >= 1)
            std::cout << "Successfully set kernel argument " << name << " at index : " << k << " with buffer : " << &buff << std::endl;
    }

    template <typename T>
    inline cl_int setKernelArg(cl::Kernel& currentKernel, int arg_idx, T& arg, const char* name = "unnamed_kernel_arg", const int verbosity = 99)
    {
        cl_int err = currentKernel.setArg(arg_idx, arg);
        if (err == CL_SUCCESS && verbosity >= 1)
            std::cout << "Successfully set kernel argument " << name << " (" << typeid(arg).name() << ") at index " << arg_idx << "\n";
        else if (err != CL_SUCCESS)
            std::cerr << "Failed to set kernel argument " << name << " (" << typeid(arg).name() << ") at index " << arg_idx << " (error " << err << ")\n";
        return err;
    }

    template <typename T>
    inline void setMapOfArgs(cl::Kernel& currentKernel, std::map<std::string, std::pair<T, int>>& map, const int verbosity = 99)
    {
        cl_int err = CL_SUCCESS;
        for (auto const& [key, val] : map)
        {
            if (verbosity >= 1)
                std::cout << "Setting kernel argument " << key << " at position " << val.second << " with value " << val.first << std::endl;
            // Use the kernel's existing argumentCount offset (if any)
            const uint32_t currentArgumentIndex = val.second + this->fractalKernel.argumentCount;
            err = setKernelArg(currentKernel, currentArgumentIndex, val.first, key.c_str(), verbosity);
            if (err != CL_SUCCESS)
            {
                std::cout << "Failed to add kernel argument " << key << " at position " << val.second << "!\n";
            }
        }
    }

    // Setting fractal and image kernel arguments
    void setFractalParameterArgs(clFractal& cf);
    void setImgKernelArguments(clFractal& cf);
    void setDefaultFractalArguments(clFractal& cf);

    // Run kernels
    void runFractalKernel(clFractal& cf) const;
    void runImgKernel(clFractal& cf) const;

    // Read image data from buffer
    void getImg(std::vector<color>& img, clFractal& cf) const;
};

void runFractalKernelAsync(clFractal& cf, clCore& cc);
void runImgKernelAsync(clFractal& cf, clCore& cc);

// cast parameter map of double to float
inline parameterMapFloat argumentMapFloatCast(parameterMapReal& realParameters)
{
    parameterMapFloat ret;
    for (auto const& [key, val] : realParameters)
        ret[key] = std::make_pair((float)val.first, val.second);
    return ret;
}

// cast parameter map of Complex<double> to Complex<float>
inline parameterMapComplexFloat argumentMapFloatCast(parameterMapComplex& complexParameters)
{
    parameterMapComplexFloat ret;
    for (auto const& [key, val] : complexParameters)
        ret[key] = std::make_pair(Complex<float>(val.first.x, val.first.y), val.second);
    return ret;
}