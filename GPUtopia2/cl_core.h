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

class clCore
{
public:
    cl::Platform platform;
    cl::Device device;
    cl::Context context;
    cl::Program program;
    cl::CommandQueue queue;
    cl::Kernel kernel;
    uint32_t kernelArgumentCount = 0;
    cl::Kernel imgKernel;
    uint32_t imgKernelArgumentCount = 0;
    bool imgKernelCompiled = false;
    cl::Buffer xBuffer, yBuffer; 
    cl::Buffer gradientBuffer;
    cl::Buffer imgIntBuffer;
    cl::Buffer imgIntRBuffer;
    cl::Buffer imgIntGBuffer;
    cl::Buffer imgIntBBuffer;
    cl::Buffer imgIntABuffer;
    cl::Buffer imgFloatBuffer;
    int currentRenderSize = 0;
    bool stop = false; // global stop flat
    clCore() 
    {
        setContext();
        if (!imgKernelCompiled)
            compileImgKernel();
    }
    void setContext();
    // fractal kernel
    void compileNewKernel(clFractal& cf);
    cl_int programError = CL_SUCCESS;
    cl_int compileError = CL_SUCCESS;
    cl_int queueError = CL_SUCCESS;
    cl_int kernelError = CL_SUCCESS;
    cl_int errSum() const
    {
        return programError + compileError + queueError + kernelError;
    }
    std::string kernelBuildLog;
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
    
    void setKernelFractalArgs(clFractal& cf);
    void setImgKernelArguments(clFractal& cf);
    void setDefaultArguments(clFractal& cf);

    // run kernels
    void runKernel(clFractal& cf) const;
    void runImgKernel(clFractal& cf) const;

    // read img data from buffer
    void getImg(std::vector<color>& img, clFractal& cf) const;

};

void runKernelAsync(clFractal& cf, clCore& cc);
void runImgKernelAsync(clFractal& cf, clCore& cc);

// void make_img(std::vector<color>& img, const int width, const int height, const formulaSettings& fs);
// void make_img2(clFractal& cf, std::vector<color>& img, const int width, const int height, const formulaSettings& fs);
