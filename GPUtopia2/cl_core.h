#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <typeinfo>
#include <chrono>
#include <CL/opencl.hpp>
#include <thread>

#include "formulaSettings.h"
#include "color.h"

#pragma OPENCL EXTENSION cl_amd_printf : enable
#pragma OPENCL EXTENSION cl_intel_printf : enable
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
    cl::Buffer xBuffer, yBuffer; // what to do about these?
    cl::Buffer imgBuffer;
    clCore() 
    {
        setContext();
    }
    void setContext();
    void compileNewKernel(clFractal& cf);
    
    template <typename T>
    cl::Buffer setBufferKernelArg(int k, T* data, size_t size, cl_mem_flags mem_flag,
        const char* name, cl_int* err_out);

    template <typename T>
    cl_int setKernelArg(int arg_idx, T& arg, const char* name = "unnamed_kernel_arg");
    
    template <typename T>
    void setMapOfArgs(std::map<std::string, std::pair<T, int>>& map);
    
    void setFractalKernelArgs(clFractal& cf);
    void setDefaultArguments(clFractal& cf);
    void runKernel(clFractal& cf) const;
    void getImg(std::vector<color>& img, clFractal& cf) const;

};

// void make_img(std::vector<color>& img, const int width, const int height, const formulaSettings& fs);
// void make_img2(clFractal& cf, std::vector<color>& img, const int width, const int height, const formulaSettings& fs);
