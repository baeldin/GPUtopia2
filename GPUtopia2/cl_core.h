#pragma once

#define CL_USE_DEPRECATED_OPENCL_2_2_APIS

#include <iostream>
#include <vector>
#include <string>
#include <typeinfo>
#include <chrono>
#include <CL/cl2.hpp>

#include "formulaSettings.h"
#include "color.h"

constexpr int WG_SIZE = 256; // Workgroup size

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
    cl::Buffer imgIntRBuffer;
    cl::Buffer imgIntGBuffer;
    cl::Buffer imgIntBBuffer;
    cl::Buffer imgIntABuffer;
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

    // methods for setting arguments
    template <typename T>
    cl::Buffer setBufferKernelArg(cl::Kernel& currentKernel, int k, T* data, size_t size, cl_mem_flags mem_flag,
        const char* name, cl_int* err_out);
    void setReusedBufferArgument(cl::Kernel& currentKernel, int k, cl::Buffer& buff, const char* name = "unnamed_reused_buffer_kernel_arg");
    template <typename T>
    cl_int setKernelArg(cl::Kernel& currentKernel, int arg_idx, T& arg, const char* name = "unnamed_kernel_arg");
    template <typename T>
    void setMapOfArgs(cl::Kernel& currentKernel, std::map<std::string, std::pair<T, int>>& map);
    
    void setFractalParameterArgs(clFractal& cf);
    void setImgKernelArguments(clFractal& cf);
    void setDefaultFractalArguments(clFractal& cf);

    // run kernels
    void runFractalKernel(clFractal& cf) const;
    void runImgKernel(clFractal& cf) const;

    // read img data from buffer
    void getImg(std::vector<color>& img, clFractal& cf) const;

};

void runFractalKernelAsync(clFractal& cf, clCore& cc);
void runImgKernelAsync(clFractal& cf, clCore& cc);

// void make_img(std::vector<color>& img, const int width, const int height, const formulaSettings& fs);
// void make_img2(clFractal& cf, std::vector<color>& img, const int width, const int height, const formulaSettings& fs);

