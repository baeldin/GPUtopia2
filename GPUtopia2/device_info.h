#pragma once
#include <string>
#include <CL/cl2.hpp>

struct DeviceInfo {
    cl::Platform platform;
    cl::Device device;
    std::string name;
    bool enabled = false;
};
