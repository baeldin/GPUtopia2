#include "cl_core.h"

struct color;

// update coordinates within the complex plane if zoom or aspect ratio is changed
void clFractalImage::updateComplexSubplane()
{
    complexSubplane.z = 4.f / zoom;
    aspectRatio = (float)size.x / (float)size.y;
    complexSubplane.w = complexSubplane.z / aspectRatio;
}


template <typename T>
cl::Buffer clCore::setBufferKernelArg(int k, T* data, size_t size, cl_mem_flags mem_flag,
    const char* name, cl_int* err_out)
{
    // Write our data set into the input array in device memory
    cl_int err = CL_SUCCESS;
    cl::Buffer buff = cl::Buffer(this->context, mem_flag, size, NULL, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error " << err << " while creating buffer " << name << std::endl;
    }
    else {
        std::cout << "Buffer " << name << " with type " << typeid(data).name() << " and size " << size << " created successfully.\n";
    }

    err = this->queue.enqueueWriteBuffer(buff, CL_TRUE, 0, size, data, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error " << err << " while enqueueing buffer " << name << std::endl;
    }
    else {
        std::cout << "Buffer write for " << name << " with type " << typeid(data).name() << " and size " << size << " enqueued successfully.\n";
    }

    err = this->kernel.setArg(k, buff);
    if (err != CL_SUCCESS) {
        std::cerr << "Failed to set kernel argument. Error code: " << err << std::endl;
    }
    else
    {
        std::cout << "Successfully set kernel argument " << name << " at index : " << k << " with buffer : " << &buff << std::endl;
    }
    return buff;
}

template <typename T>
cl_int clCore::setKernelArg(int arg_idx, T& arg, const char* name)
{
    cl_int err = kernel.setArg(arg_idx, arg);
    if (err == CL_SUCCESS)
        std::cout << "Successfully set kernel argument " << name << " (" << typeid(arg).name() << ") at index " << arg_idx << "\n";
    else
        std::cerr << "Failed to set kernel argument " << name << " (" << typeid(arg).name() << ") at index " << arg_idx << "\n";
    return err;
}


void clCore::setContext()
{
    cl_int perr = cl::Platform::get(&this->platform);
    std::cout << "Platform err: " << perr << "\n";
    std::cout << "Using platform: " << this->platform.getInfo<CL_PLATFORM_NAME>() << "\n";
    std::vector<cl::Device> all_devices;
    this->platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
    this->device = all_devices[0];
    std::cout << "Using device: " << this->device.getInfo<CL_DEVICE_NAME>() << "\n";
    cl_int err_context;
    cl::Context context(this->device, nullptr, nullptr, nullptr, &err_context);
    std::cout << err_context << "\n";
    if (err_context != CL_SUCCESS) {
        std::cerr << "Failed to create OpenCL context. Error code: " << err_context << std::endl;
    }
    this->context = context;
}

void clCore::compileNewKernel(clFractal& cf)
{
    cl::Program::Sources sources;
    std::string kernelCode = cf.fullCLcode;
    std::vector<std::string> programStrings;
    programStrings.push_back(kernelCode);
    cl_int programErr;
    this->program = cl::Program(this->context, programStrings, &programErr);
    std::cout << programErr << "\n";
    cl_int buildErr = CL_SUCCESS;
    buildErr = this->program.build({ this->device }); // "-cl-std=CL2.0");
    if (buildErr != CL_SUCCESS) {
        std::string buildLog;
        this->program.getBuildInfo(this->device, CL_PROGRAM_BUILD_LOG, &buildLog);
        std::cerr << "Build failed; error code: " << buildErr
            << ", build log:\n" << buildLog << std::endl;
    }
    cl_int err;
    this->queue = cl::CommandQueue(this->context, this->device, 0, &err);
    if (err != CL_SUCCESS) {
        std::cout << "Failed to create command queue. Error code: " << err << std::endl;
    }
    std::string kernelFunctionName = "computeLoop"; // Replace with your actual kernel function name
    this->kernel = cl::Kernel(this->program, kernelFunctionName.c_str(), &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Failed to create kernel. Error code: " << err << std::endl;
    }
    else {
        std::cout << "Kernel created successfully.\n";
    }
}

template <typename T>
void clCore::setMapOfArgs(std::map<std::string, std::pair<T, int>>& map)
{
    cl_int err = CL_SUCCESS;
    for (auto const& [key, val] : map)
    {
        std::cout << "Setting kernel argument " << key << " at position " << val.second << " with value " << val.first << std::endl;
        err = setKernelArg(val.second, val.first, key.c_str());
        if (err != CL_SUCCESS)
        {
            std::cout << "Failed to add kernel argument " << key << " at position " << val.second << "!\n";
        }
    }
}

void clCore::setDefaultArguments(clFractal& cf)
{
    std::vector<int>xx(cf.image.size.x * cf.image.size.y);
    std::vector<int>yy(cf.image.size.x * cf.image.size.y);
    const int npixels = cf.image.size.x * cf.image.size.y;
    int pixelIndex = 0;
    for (int y = 0; y < cf.image.size.y; y++) {
        for (int x = 0; x < cf.image.size.x; x++) {
            pixelIndex = y * cf.image.size.x + x;
            xx[pixelIndex] = x;
            yy[pixelIndex] = y;
        }
    }
    std::vector<color> imgColors(npixels, 0);
    cl_int3 sampling = { 0, fibonacci_number(cf.image.quality), fibonacci_number(cf.image.quality) };
    cl_int err;
    std::cout << "xx size is " << xx.size() << std::endl;
    this->xBuffer = setBufferKernelArg(0, xx.data(), 
        sizeof(int) * npixels, CL_MEM_READ_ONLY, "xx", &err);
    this->yBuffer = setBufferKernelArg(1, yy.data(), 
        sizeof(int) * npixels, CL_MEM_READ_ONLY, "yy", &err);
    err = setKernelArg(2, cf.image.size, "image_size");
    err = setKernelArg(3, cf.image.complexSubplane, "complex_subplane");
    err = setKernelArg(4, sampling, "sampling_info");
    err = setKernelArg(5, cf.maxIter, "iterations");
    err = setKernelArg(6, cf.bailout, "bailout");
    this->imgBuffer = setBufferKernelArg(7, imgColors.data(), 
        sizeof(cl_float4) * npixels, CL_MEM_WRITE_ONLY, "img", &err);
}

void clCore::setFractalKernelArgs(clFractal& cf)
{
    setMapOfArgs(cf.params.fractalParameterMaps.integerParameters);
    setMapOfArgs(cf.params.fractalParameterMaps.floatParameters);
    setMapOfArgs(cf.params.coloringParameterMaps.integerParameters);
    setMapOfArgs(cf.params.coloringParameterMaps.floatParameters);
}

void clCore::runKernel(clFractal& cf) const
{
    cl_int local_size = WG_SIZE;
    cl_int global_size = ceil(
        cf.image.size.x * cf.image.size.y / (float)local_size) * local_size;
    cl_int err = CL_SUCCESS;
    cl::NDRange global_range = cl::NDRange(global_size);
    cl::NDRange local_range = cl::NDRange(local_size);
    err = this->queue.enqueueNDRangeKernel(this->kernel, cl::NullRange, global_range, local_range);
    if (err != CL_SUCCESS) {
        std::cerr << "Failed to enqueue kernel. Error code: " << err << std::endl;
    }
    this->queue.finish();
}

void clCore::getImg(std::vector<color>& img, clFractal& cf) const
{
    const int npixels = cf.image.size.x * cf.image.size.y;
    queue.enqueueReadBuffer(this->imgBuffer, CL_TRUE, 0, sizeof(cl_float4) * npixels, img.data());
}

//    
//void make_img2(clFractal& cf, std::vector<color>& img, const int width, const int height, const formulaSettings& fs)
//{
//    cl::Platform default_platform;
//    cl_int perr = cl::Platform::get(&default_platform);
//    std::cout << "Platform err: " << perr << "\n";
//    std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";
//    std::vector<cl::Device> all_devices;
//    default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
//    cl::Device default_device = all_devices[0];
//    std::cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>() << "\n";
//    cl_int err_context;
//    cl::Context context(default_device, nullptr, nullptr, nullptr, &err_context);
//    std::cout << err_context << "\n";
//    if (err_context != CL_SUCCESS) {
//        std::cerr << "Failed to create OpenCL context. Error code: " << err_context << std::endl;
//    }
//    cl::Program::Sources sources;
//    std::string mb_kernel = cf.fullCLcode;
//
//    std::cout << "Preparing buffers and kernel\n";
//    auto t_buff1 = std::chrono::high_resolution_clock::now();
//    std::vector<std::string> programStrings;
//    programStrings.push_back(mb_kernel);
//    cl_int programErr;
//    cl::Program mbProgram(context, programStrings, &programErr);
//    std::cout << programErr << "\n";
//    cl_int buildErr = CL_SUCCESS;
//    // Data and buffers    
//    // Define Mandelbrot Settings
//    int iterations = fs.maxIter;
//    float x_min = -2.5;
//    float x_max = 1.5;
//    float y_min = -1.5f;
//    float y_max = 1.5f;
//    float zoom = 1.;
//    float bailout = 128.f;
//    cl_float4  cs = { fs.centerx, fs.centery, 3. / fs.zoom, 3. / fs.zoom / (float)width * (float)height };
//    // generate x and y array for pixel coordinates and fill them with a loop
//    int npixels = width * height;
//    std::cout << "Generating vectors for " << width << " x " << height << " Pixels image (" << npixels << ")\n";
//    std::vector<int>xx(npixels);
//    std::vector<int>yy(npixels);
//    int pixelIndex = 0;
//    for (int y = 0; y < height; y++) {
//        for (int x = 0; x < width; x++) {
//            pixelIndex = y * width + x;
//            xx[pixelIndex] = x;
//            yy[pixelIndex] = y;
//        }
//    }
//
//    // set image size, location in complex plane, samples
//    cl_int2 image_size = { width, height };
//    cl_float4 complex_subplane = cs; // {0.5f * (x_min + x_max), 0.5f * (y_min + y_max), x_max - x_min, (x_max - x_min) / (float)width * (float)height };
//    cl_int3 sampling_info = { 0, fibonacci_number(fs.quality), fibonacci_number(fs.quality) }; // all 8 samples of Fib 8 at once
//    std::cout << " " << sampling_info.x << " " << sampling_info.y << " " << sampling_info.z << std::endl;
//    // define return values
//    std::vector<cl_float4>colors(npixels);
//
//
//    buildErr = mbProgram.build({ default_device }); // "-cl-std=CL2.0");
//    if (buildErr != CL_SUCCESS) {
//        std::string buildLog;
//        mbProgram.getBuildInfo(default_device, CL_PROGRAM_BUILD_LOG, &buildLog);
//        std::cerr << "Build failed; error code: " << buildErr
//            << ", build log:\n" << buildLog << std::endl;
//    }
//    // std::vector<float>ret(N);
//    cl_int err;
//    cl::CommandQueue queue(context, default_device, 0, &err);
//    if (err != CL_SUCCESS) {
//        std::cout << "Failed to create command queue. Error code: " << err << std::endl;
//    }
//    std::string kernelFunctionName = "computeLoop"; // Replace with your actual kernel function name
//    cl::Kernel kernel(mbProgram, kernelFunctionName.c_str(), &err);
//    if (err != CL_SUCCESS) {
//        std::cerr << "Failed to create kernel. Error code: " << err << std::endl;
//    }
//    else {
//        std::cout << "Kernel created successfully.\n";
//    }
//    addMappedKernelArgs(kernel, cf.params);
//    auto t_buff2 = std::chrono::high_resolution_clock::now() - t_buff1;
//    std::cout << "This took " << t_buff2.count() / 1000000000. << " seconds.\n";
//    std::cout << "Running kernel...\n";
//    auto t_run1 = std::chrono::high_resolution_clock::now();
//    cl_int local_size = WG_SIZE;
//    cl_int global_size = ceil(npixels / (float)local_size) * local_size;
//    err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(global_size), cl::NDRange(local_size));
//    if (err != CL_SUCCESS) {
//        std::cerr << "Failed to enqueue kernel. Error code: " << err << std::endl;
//    }
//    queue.finish();
//    auto t_run2 = std::chrono::high_resolution_clock::now() - t_run1;
//    std::cout << "This took " << t_run2.count() / 1000000000. << " seconds.\n";
//
//    std::vector<float> bout(npixels, 0);
//    // queue.enqueueReadBuffer(dret, CL_TRUE, 0, sizeof(float) * N, test.data());
//    queue.enqueueReadBuffer(dimg, CL_TRUE, 0, sizeof(cl_float4) * npixels, img.data());
//    //for (int ii = 0; ii < width * height; ii++) {
//    //    std::cout << ii << ": (" << img[ii].r << ", " << img[ii].g << ", " << img[ii].b << ", " << img[ii].a << ")\n";
//    //}
//    //std::cout << "Saving image...\n";
//    //auto s_run1 = std::chrono::high_resolution_clock::now();
//    //save_to_png(img, nx, ny, (char*)"cltest.png");
//    //auto s_run2 = std::chrono::high_resolution_clock::now() - s_run1;
//    //std::cout << "This took " << s_run2.count() / 1000000000. << " secondsl\n";
//}


//
//
//void make_img(std::vector<color>& img, const int width, const int height, const formulaSettings& fs)
//{
//    cl::Platform default_platform;
//    cl_int perr = cl::Platform::get(&default_platform);
//    std::cout << "Platform err: " << perr << "\n";
//    std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";
//    std::vector<cl::Device> all_devices;
//    default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
//    cl::Device default_device = all_devices[0];
//    std::cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>() << "\n";
//    cl_int err_context;
//    cl::Context context(default_device, nullptr, nullptr, nullptr, &err_context);
//    std::cout << err_context << "\n";
//    if (err_context != CL_SUCCESS) {
//        std::cerr << "Failed to create OpenCL context. Error code: " << err_context << std::endl;
//    }
//    cl::Program::Sources sources;
//    std::string mb_kernel{ R"CLC(
//// Computes the Mandelbrot Set to N Iterations
//// Set some constants
//const float phi = 1.618033988749f;
//const float pi = 3.1415926535897f;
//const float twopi = 6.28318530718f;
//const float deg2rad = pi / 180.f;
//const float rad2deg = 180.f / pi;
//
//float fracf(const float x) { return x - floor(x); }
//float absf(const float x) { return x * sign(x); }
//float2 absf2(const float2 x) { return x * sign(x); }
//
//float2 fib_lattice(const int k, const int N) { return (float2)((float)k * phi, (float)k / (float)N); }
//
//float tent(float x) {
//    x = 2.f * x - 1.f;
//    if (x == 0) { return 0.f; }
//    return x / sqrt(absf(x)) - sign(x);
//}
//
//uint lowbias32(uint x)
//{
//    x ^= x >> 16;
//    x *= 0x7feb352dU;
//    x ^= x >> 15;
//    x *= 0x846ca68bU;
//    x ^= x >> 16;
//    return x;
//}
//
//const uint uint_max = 0xffffffffu;
//float tofloat(uint x) {
//    return (float)x / (float)uint_max;
//}
//
//float2 get_complex_coordinates(const float2 xy, const int2 img_size, const float4 cz)
//{
//    return (float2)(
//        cz.x + (xy.x / (float)img_size.x - 0.5f) * cz.z,
//        cz.y + (xy.y / (float)img_size.y - 0.5f) * cz.w);
//}
//
//__kernel void solve_mandelbrot(
//    __global int const* xx,
//    __global int const* yy,
//    int2 image_size,            // nx, ny
//    float4 complex_subplane,    // centerx, centery, width, height
//    int3 sampling,              // k_min, k_max, N_samples
//    int iterations,
//    __global float4* colors)
//{
//    // Get Parallel Index
//    unsigned int i = get_global_id(0);
//
//    const int x = xx[i]; // Real Component
//    const int y = yy[i]; // Imaginary Component
//    const int pixelIdx = image_size.x * y + x;
//    const int sample_count = sampling.y - sampling.x;
//    int offset_fac = min(1, sampling.z - 1);
//    // -0.156844471694257101941 - 0.649707745759247905171i;
//    // -0.749 - 0.15i
//    for (int s = sampling.x; s < sampling.y; s++)
//    //for (int s = 0; s < 1; s++)
//
//    {
//        int n = 0;       // Tracks Color Information
//        float d_min = 99.;
//        float d_max = 0.;
//        float d = 99.;
//
//        float2 sample_position = (float2)(x, y) + offset_fac * (float2)(
//            tent(fracf((float)s * phi               + tofloat(lowbias32((uint)(2 * pixelIdx    ))))),
//            tent(fracf((float)s / (float)sampling.z + tofloat(lowbias32((uint)(2 * pixelIdx + 1))))));
//        const float2 z0 = get_complex_coordinates(sample_position, image_size, complex_subplane);
//        float2 z = z0; // + (float2)(-0.3f, 0.25f);
//        while ((z.x * z.x + z.y * z.y <= 40) && n < iterations)
//        {
//            //z = absf2(z);
//            float xtemp = z.x * z.x - z.y * z.y - 0.73f; // + z0.x;
//            z.y = 2 * z.x * z.y - 0.34f; //z0.y;
//            z.x = xtemp;
//            n++;
//            // some orbit-traps-like magic
//            d = pow(absf(length(z) - 1.20710678119f), 2.f);
//            if (d < d_min) { d_min = d; }
//            d = pow(absf(length(z+(float2)(0.5, 0.5)) - 0.5f), 2.f);
//            if (d < d_min) { d_min = d; }
//            d = pow(absf(length(z+(float2)(-0.5, 0.5)) - 0.5f), 2.f);
//            if (d < d_min) { d_min = d; }
//            d = pow(absf(length(z+(float2)(0.5, -0.5)) - 0.5f), 2.f);
//            if (d < d_min) { d_min = d; }
//            d = pow(absf(length(z+(float2)(-0.5, -0.5)) - 0.5f), 2.f);
//            if (d < d_min) { d_min = d; }
//        }
//        // colors[i].xyzw += (float4)(0.5 + 0.5 * sin(20 * d_min), 0.5 + 0.5 * sin(15*d_min / d_max), 0.5 + 0.5 * sin(20 * d_max), 1.f);
//        colors[i].xyzw += (float4)(
//            //0.5f + 0.25f * cos(0.1 * n) + 0.25 * cos(1 * length(z)), 
//            //0.5f + 0.25f * cos(0.1 * n) + 0.25 * cos(1 * length(z)), 
//            //0.5f + 0.25f * cos(0.1 * n) + 0.25 * cos(1 * length(z)), 1.f);
//            // initial test img :)
//            1. - 2.f / pi * atan(230000/(0.1*n)* d_min), 
//            1. - 2.f / pi * atan(150000/(0.1*n)* d_min), 
//            1. - 2.f / pi * atan(48000/(0.1*n)* d_min), 1.f);
//            //n < iterations ? 1. - 2.f / pi * atan(0.033 * 23000 * d_min) : 0.f, 
//            //n < iterations ? 1. - 2.f / pi * atan(0.033 * 19000 * d_min) : 0.f, 
//            //n < iterations ? 1. - 2.f / pi * atan(0.033 * 4800 * d_min) : 0.f, 1.f);
//    }
//    colors[i].xyzw /= (float4)(sample_count, sample_count, sample_count, 1.f);
//    //colors[i].xyzw = (float4)((float)sampling.x, (float)sampling.y, (float)sampling.z, 1.);
//}
//    )CLC" };
//    std::cout << "Preparing buffers and kernel\n";
//    auto t_buff1 = std::chrono::high_resolution_clock::now();
//    std::vector<std::string> programStrings;
//    programStrings.push_back(mb_kernel);
//    cl_int programErr;
//    cl::Program mbProgram(context, programStrings, &programErr);
//    std::cout << programErr << "\n";
//    cl_int buildErr = CL_SUCCESS;
//    // Data and buffers    
//    // Define Mandelbrot Settings
//    int iterations = fs.maxIter;
//    float x_min = -2.5;
//    float x_max = 1.5;
//    float y_min = -1.5f;
//    float y_max = 1.5f;
//    float zoom = 1.;
//    cl_float4  cs = { fs.centerx, fs.centery, 3. / fs.zoom, 3. / fs.zoom / (float)width * (float)height };
//    // generate x and y array for pixel coordinates and fill them with a loop
//    int npixels = width * height;
//    std::cout << "Generating vectors for " << width << " x " << height << " Pixels image (" << npixels << ")\n";
//    std::vector<int>xx(npixels);
//    std::vector<int>yy(npixels);
//    int pixelIndex = 0;
//    for (int y = 0; y < height; y++) {
//        for (int x = 0; x < width; x++) {
//            pixelIndex = y * width + x;
//            xx[pixelIndex] = x;
//            yy[pixelIndex] = y;
//        }
//    }
//
//    // set image size, location in complex plane, samples
//    cl_int2 image_size = { width, height };
//    cl_float4 complex_subplane = cs; // {0.5f * (x_min + x_max), 0.5f * (y_min + y_max), x_max - x_min, (x_max - x_min) / (float)width * (float)height };
//    cl_int3 sampling_info = { 0, fibonacci_number(fs.quality), fibonacci_number(fs.quality)}; // all 8 samples of Fib 8 at once
//    std::cout << sampling_info.z << std::endl;
//    // define return values
//    std::vector<cl_float4>colors(npixels);
//
//
//    buildErr = mbProgram.build({ default_device }); // "-cl-std=CL2.0");
//    if (buildErr != CL_SUCCESS) {
//        std::string buildLog;
//        mbProgram.getBuildInfo(default_device, CL_PROGRAM_BUILD_LOG, &buildLog);
//        std::cerr << "Build failed; error code: " << buildErr
//            << ", build log:\n" << buildLog << std::endl;
//    }
//    // std::vector<float>ret(N);
//    cl_int err;
//    cl::CommandQueue queue(context, default_device, 0, &err);
//    if (err != CL_SUCCESS) {
//        std::cout << "Failed to create command queue. Error code: " << err << std::endl;
//    }
//    std::string kernelFunctionName = "solve_mandelbrot"; // Replace with your actual kernel function name
//    cl::Kernel kernel(mbProgram, kernelFunctionName.c_str(), &err);
//    if (err != CL_SUCCESS) {
//        std::cerr << "Failed to create kernel. Error code: " << err << std::endl;
//    }
//    else {
//        std::cout << "Kernel created successfully.\n";
//    }
//    cl::Buffer dxx = addBufferKernelArg(0, xx.data(), sizeof(int) * npixels, CL_MEM_READ_ONLY, context, queue, kernel, "xx");
//    cl::Buffer dyy = addBufferKernelArg(1, yy.data(), sizeof(int) * npixels, CL_MEM_READ_ONLY, context, queue, kernel, "yy");
//    err = addKernelArg(2, image_size      , kernel, "image_size"      );
//    err = addKernelArg(3, complex_subplane, kernel, "complex_subplane");
//    err = addKernelArg(4, sampling_info   , kernel, "sampling_info"   );
//    err = addKernelArg(5, iterations      , kernel, "iterations"      );
//    cl::Buffer dimg = addBufferKernelArg(6, colors.data(), sizeof(cl_float4) * npixels, CL_MEM_WRITE_ONLY, context, queue, kernel, "img");
//    auto t_buff2 = std::chrono::high_resolution_clock::now() - t_buff1;
//    std::cout << "This took " << t_buff2.count() / 1000000000. << " seconds.\n";
//    std::cout << "Running kernel...\n";
//    auto t_run1 = std::chrono::high_resolution_clock::now();
//    cl_int local_size = WG_SIZE;
//    cl_int global_size = ceil(npixels / (float)local_size) * local_size;
//    err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(global_size), cl::NDRange(local_size));
//    if (err != CL_SUCCESS) {
//        std::cerr << "Failed to enqueue kernel. Error code: " << err << std::endl;
//    }
//    queue.finish();
//    auto t_run2 = std::chrono::high_resolution_clock::now() - t_run1;
//    std::cout << "This took " << t_run2.count() / 1000000000. << " seconds.\n";
//
//    std::vector<float> bout(npixels, 0);
//    // queue.enqueueReadBuffer(dret, CL_TRUE, 0, sizeof(float) * N, test.data());
//    queue.enqueueReadBuffer(dimg, CL_TRUE, 0, sizeof(cl_float4) * npixels, img.data());
//    //for (int ii = 0; ii < width * height; ii++) {
//    //    std::cout << ii << ": (" << img[ii].r << ", " << img[ii].g << ", " << img[ii].b << ", " << img[ii].a << ")\n";
//    //}
//    //std::cout << "Saving image...\n";
//    //auto s_run1 = std::chrono::high_resolution_clock::now();
//    //save_to_png(img, nx, ny, (char*)"cltest.png");
//    //auto s_run2 = std::chrono::high_resolution_clock::now() - s_run1;
//    //std::cout << "This took " << s_run2.count() / 1000000000. << " secondsl\n";
//}
