#include "cl_core.h"
#include "cl_handler.h" // readCLFragmentFromFile

struct color;

void clCore::setContext()
{
    cl_int perr = cl::Platform::get(&this->platform);
    std::cout << "Platform err: " << perr << "\n";
    std::cout << "Using platform: " << this->platform.getInfo<CL_PLATFORM_NAME>() << "\n";
    std::vector<cl::Device> all_devices;
    this->platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
    if (all_devices.empty()) {
        throw std::runtime_error("No OpenCL devices found.");
    }
    this->device = all_devices[0];
    std::cout << "Using device: " << this->device.getInfo<CL_DEVICE_NAME>() << "\n";
    cl_int err_context;
    cl::Context ctx(this->device, nullptr, nullptr, nullptr, &err_context);
    std::cout << "Context creation error code: " << err_context << "\n";
    if (err_context != CL_SUCCESS) {
        std::cerr << "Failed to create OpenCL context. Error code: " << err_context << std::endl;
    }
    this->context = ctx;
}

void clCore::setQueue()
{
    // Query for the extension support first.
    if (this->device.getInfo<CL_DEVICE_EXTENSIONS>().find("cl_khr_priority_hints") != std::string::npos)
    {
        cl_queue_properties props[] = {
            CL_QUEUE_PRIORITY_KHR, CL_QUEUE_PRIORITY_LOW_KHR,
            0
        };
        // Create the command queue using the C API.
        cl_command_queue cq = clCreateCommandQueueWithProperties(context(), device(), props, &this->queueError);
        if (this->queueError != CL_SUCCESS)
        {
            std::cerr << "clCreateCommandQueueWithProperties failed with error: " << this->queueError << "\n";
            // Fall back to a default queue.
            this->queue = cl::CommandQueue(context, device, 0, &this->queueError);
        }

        // Wrap the cl_command_queue in a cl::CommandQueue.
        this->queue = cl::CommandQueue(cq);
    }
    else
    {
        // Extension not supported; create a default command queue.
        this->queue = cl::CommandQueue(this->context, this->device, 0, &this->queueError);
        if (this->queueError != CL_SUCCESS) {
            std::cout << "Failed to create command queue. Error code: " << this->queueError << std::endl;
        }
    }
}

cl_int clCore::compileNewKernel(clKernelContainer& kc, const std::string& fullCLcode,
    const std::string kernelFunctionName)
{
    std::vector<std::string> programStrings{ fullCLcode };
    kc.program = cl::Program(this->context, programStrings, &kc.errors.programError);
    kc.errors.compileError = kc.program.build({ this->device }, "-cl-std=CL2.0");
    if (kc.errors.compileError != CL_SUCCESS) {
        kc.program.getBuildInfo(this->device, CL_PROGRAM_BUILD_LOG, &kc.buildLog);
        return kc.errors.compileError;
    }
    kc.kernel = cl::Kernel(kc.program, kernelFunctionName.c_str(), &kc.errors.kernelError);
    if (kc.errors.kernelError != CL_SUCCESS) {
        std::cerr << "Failed to create kernel. Error code: " << kc.errors.kernelError << std::endl;
        return kc.errors.kernelError;
    }
    else {
        std::cout << kernelFunctionName << " created successfully.\n";
    }
    kc.argumentCount = 0;
    kc.need = false;
    return CL_SUCCESS;
}

void clCore::resetCore(clLayerState& ls)
{
    this->queueError = CL_SUCCESS;
    ls.imgKernel.errors.reset();
    ls.fractalKernel.errors.reset();
    ls.stop = false;
}

void clCore::setDefaultFractalArguments(clLayerState& ls, clFractal& cf)
{
    ls.currentRenderSize = cf.image.size.x * cf.image.size.y;
    cf.imgIntRGBAData.resize(4 * ls.currentRenderSize, 0);
    cl_int3 sampling = { 0, fibonacci_number(cf.image.targetQuality), fibonacci_number(cf.image.targetQuality) };
    cl_int err;
    err = setKernelArg(ls.fractalKernel.kernel, static_cast<int>(FractalKernelArg::IMAGE_SIZE), cf.image.size, "image_size", cf.verbosity);
    if (cf.useDouble)
    {
        const cl_double4 complexSubplane = { cf.image.center.x, cf.image.center.y, cf.image.span.x, cf.image.span.y };
        err = setKernelArg(ls.fractalKernel.kernel, static_cast<int>(FractalKernelArg::COMPLEX_SUBPLANE), complexSubplane, "complex_subplane", cf.verbosity);
        const cl_double2 rot = { cf.image.rotation().x, cf.image.rotation().y };
        err = setKernelArg(ls.fractalKernel.kernel, static_cast<int>(FractalKernelArg::ROTATION), rot, "rotation", cf.verbosity);
    }
    else
    {
        const cl_float4 complexSubplane = { static_cast<float>(cf.image.center.x), static_cast<float>(cf.image.center.y), static_cast<float>(cf.image.span.x), static_cast<float>(cf.image.span.y) };
        err = setKernelArg(ls.fractalKernel.kernel, static_cast<int>(FractalKernelArg::COMPLEX_SUBPLANE), complexSubplane, "complex_subplane", cf.verbosity);
        const cl_float2 rot = { static_cast<float>(cf.image.rotation().x), static_cast<float>(cf.image.rotation().y) };
        err = setKernelArg(ls.fractalKernel.kernel, static_cast<int>(FractalKernelArg::ROTATION), rot, "rotation", cf.verbosity);
    }
    err = setKernelArg(ls.fractalKernel.kernel, static_cast<int>(FractalKernelArg::ITERATIONS), cf.maxIter, "iterations", cf.verbosity);
    err = setKernelArg(ls.fractalKernel.kernel, static_cast<int>(FractalKernelArg::BAILOUT), cf.bailout, "bailout", cf.verbosity);
    err = setKernelArg(ls.fractalKernel.kernel, static_cast<int>(FractalKernelArg::GRADIENT_LENGTH), cf.gradient.fineLength, "gradient_length", cf.verbosity);
    ls.gradientBuffer = setBufferKernelArg(ls.fractalKernel.kernel, static_cast<int>(FractalKernelArg::GRADIENT_COLORS),
        cf.gradient.fineColors.data(), sizeof(cl_float) * cf.gradient.fineLength * 4, CL_MEM_READ_ONLY, "gradient_colors", &err, cf.verbosity);
    ls.imgIntRGBABuffer = setBufferKernelArg(ls.fractalKernel.kernel, static_cast<int>(FractalKernelArg::IMG_INT_RGBA),
        cf.imgIntRGBAData.data(), 4 * sizeof(cl_int) * ls.currentRenderSize, CL_MEM_WRITE_ONLY, "img", &err, cf.verbosity);
    err = setKernelArg(ls.fractalKernel.kernel, static_cast<int>(FractalKernelArg::FLAME_POINT_SELECTION), cf.flamePointSelection, "flamePointSelection", cf.verbosity);
    err = setKernelArg(ls.fractalKernel.kernel, static_cast<int>(FractalKernelArg::FLAME_WARMUP), cf.flameWarmup, "flameWarmup", cf.verbosity);
    err = setKernelArg(ls.fractalKernel.kernel, static_cast<int>(FractalKernelArg::FRACTAL_MODE), cf.mode, "fractal mode", cf.verbosity);
    ls.fractalKernel.argumentCount = 12;
}

void clCore::setFractalParameterArgs(clLayerState& ls, clFractal& cf)
{
    int ac = ls.fractalKernel.argumentCount;
    setMapOfArgs(ls.fractalKernel.kernel, cf.params.fractalParameterMaps.integerParameters, ac, cf.verbosity);
    setMapOfArgs(ls.fractalKernel.kernel, cf.params.insideColoringParameterMaps.integerParameters, ac, cf.verbosity);
    setMapOfArgs(ls.fractalKernel.kernel, cf.params.outsideColoringParameterMaps.integerParameters, ac, cf.verbosity);
    setMapOfArgs(ls.fractalKernel.kernel, cf.params.fractalParameterMaps.enumParameters, ac, cf.verbosity);
    setMapOfArgs(ls.fractalKernel.kernel, cf.params.insideColoringParameterMaps.enumParameters, ac, cf.verbosity);
    setMapOfArgs(ls.fractalKernel.kernel, cf.params.outsideColoringParameterMaps.enumParameters, ac, cf.verbosity);
    if (cf.useDouble)
    {
        setMapOfArgs(ls.fractalKernel.kernel, cf.params.fractalParameterMaps.realParameters, ac, cf.verbosity);
        setMapOfArgs(ls.fractalKernel.kernel, cf.params.fractalParameterMaps.complexParameters, ac, cf.verbosity);
        setMapOfArgs(ls.fractalKernel.kernel, cf.params.outsideColoringParameterMaps.realParameters, ac, cf.verbosity);
        setMapOfArgs(ls.fractalKernel.kernel, cf.params.outsideColoringParameterMaps.complexParameters, ac, cf.verbosity);
        setMapOfArgs(ls.fractalKernel.kernel, cf.params.insideColoringParameterMaps.realParameters, ac, cf.verbosity);
        setMapOfArgs(ls.fractalKernel.kernel, cf.params.insideColoringParameterMaps.complexParameters, ac, cf.verbosity);
    }
    else
    {
        parameterMapFloat floatArgumentMapF = argumentMapFloatCast(cf.params.fractalParameterMaps.realParameters);
        parameterMapComplexFloat complexFloatArgumentMapF = argumentMapFloatCast(cf.params.fractalParameterMaps.complexParameters);
        parameterMapFloat floatArgumentMapCO = argumentMapFloatCast(cf.params.outsideColoringParameterMaps.realParameters);
        parameterMapComplexFloat complexFloatArgumentMapCO = argumentMapFloatCast(cf.params.outsideColoringParameterMaps.complexParameters);
        parameterMapFloat floatArgumentMapCI = argumentMapFloatCast(cf.params.insideColoringParameterMaps.realParameters);
        parameterMapComplexFloat complexFloatArgumentMapCI = argumentMapFloatCast(cf.params.insideColoringParameterMaps.complexParameters);
        setMapOfArgs(ls.fractalKernel.kernel, floatArgumentMapF, ac, cf.verbosity);
        setMapOfArgs(ls.fractalKernel.kernel, complexFloatArgumentMapF, ac, cf.verbosity);
        setMapOfArgs(ls.fractalKernel.kernel, floatArgumentMapCO, ac, cf.verbosity);
        setMapOfArgs(ls.fractalKernel.kernel, complexFloatArgumentMapCO, ac, cf.verbosity);
        setMapOfArgs(ls.fractalKernel.kernel, floatArgumentMapCI, ac, cf.verbosity);
        setMapOfArgs(ls.fractalKernel.kernel, complexFloatArgumentMapCI, ac, cf.verbosity);
    }
}

void clCore::compileFractalKernel(clLayerState& ls, const std::string fullCLcode)
{
    this->compileNewKernel(ls.fractalKernel, fullCLcode, "computeLoop");
}

void compileFractalKernelAsync(clCore& cc, clLayerState& ls, std::string& code, bool& compiling, bool& runKernel)
{
    cc.compileFractalKernel(ls, code);
    compiling = false;
    runKernel = true;
}

void clCore::runFractalKernel(clLayerState& ls, clFractal& cf) const
{
    cl_int local_size = WG_SIZE;
    cl_int global_size = static_cast<cl_int>(std::ceil(cf.image.size.x * cf.image.size.y / static_cast<float>(local_size)) * local_size);
    cl_int err = CL_SUCCESS;
    cl::NDRange global_range(global_size);
    cl::NDRange local_range(local_size);
    auto t1 = std::chrono::high_resolution_clock::now();
    err = this->queue.enqueueNDRangeKernel(ls.fractalKernel.kernel, cl::NullRange, global_range, local_range);
    if (err != CL_SUCCESS) {
        std::cerr << "Failed to enqueue kernel. Error code: " << err << std::endl;
    }
    this->queue.finish();
    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    {
        std::lock_guard<std::mutex> guard(timingMutex);
        cf.timings.push_back(static_cast<float>(time_span.count()));
    }
}

void clCore::compileImgKernel(clLayerState& ls)
{
    const std::string img_processing_kernel = readCLFragmentFromFile("clFragments/imgKernel.cl");
    this->compileNewKernel(ls.imgKernel, img_processing_kernel, "imgProcessing");
}

void clCore::setImgKernelArguments(clLayerState& ls, clFractal& cf)
{
    cl_int err;
    cf.imgData.resize(cf.image.size.x * cf.image.size.y);
    setReusedBufferArgument(ls.imgKernel.kernel, 0, ls.imgIntRGBABuffer, "intImgBuffer", cf.verbosity);
    ls.imgFloatBuffer = setBufferKernelArg(ls.imgKernel.kernel, 2, cf.imgData.data(), sizeof(cl_float4) * ls.currentRenderSize, CL_MEM_WRITE_ONLY, "imgFloatColorValues", &err, cf.verbosity);
    err = setKernelArg(ls.imgKernel.kernel, 4, cf.mode, "image processing mode (escape time/flame)", cf.verbosity);
    err = setKernelArg(ls.imgKernel.kernel, 5, cf.frs.brightness, "flame render brightness", cf.verbosity);
    err = setKernelArg(ls.imgKernel.kernel, 6, cf.frs.gamma, "flame render gamma", cf.verbosity);
    err = setKernelArg(ls.imgKernel.kernel, 7, cf.frs.vibrancy, "flame render vibrancy", cf.verbosity);
    ls.imgKernel.argumentCount = 8;
}

void clCore::runImgKernel(clLayerState& ls, clFractal& cf) const
{
    cl_int local_size = WG_SIZE;
    cl_int global_size = static_cast<cl_int>(std::ceil(cf.image.size.x * cf.image.size.y / static_cast<float>(local_size)) * local_size);
    cl_int err = CL_SUCCESS;
    cl::NDRange global_range(global_size);
    cl::NDRange local_range(local_size);
    err = this->queue.enqueueNDRangeKernel(ls.imgKernel.kernel, cl::NullRange, global_range, local_range);
    if (err != CL_SUCCESS) {
        std::cerr << "Failed to enqueue image kernel. Error code: " << err << std::endl;
    }
    this->queue.finish();
    if (cf.vomit)
    {
        std::vector<uint32_t> imgInt(4 * ls.currentRenderSize, 0);
        queue.enqueueReadBuffer(ls.imgIntRGBABuffer, CL_TRUE, 0, sizeof(cl_int4) * ls.currentRenderSize, imgInt.data());
        for (int ii = 0; ii < ls.currentRenderSize * 4; ii++)
            std::cout << imgInt[ii] << " ";
        std::cout << std::endl;
        std::cout << " ============================================== " << std::endl;
        std::vector<float> imgFloat(4 * ls.currentRenderSize, 0);
        queue.enqueueReadBuffer(ls.imgFloatBuffer, CL_TRUE, 0, sizeof(cl_float4) * ls.currentRenderSize, imgFloat.data());
        for (int ii = 0; ii < ls.currentRenderSize * 4; ii++)
            std::cout << imgFloat[ii] << " ";
        std::cout << std::endl;
    }
}

void clCore::getImg(clLayerState& ls, std::vector<color>& img, clFractal& cf) const
{
    queue.enqueueReadBuffer(ls.imgFloatBuffer, CL_TRUE, 0, sizeof(cl_float4) * ls.currentRenderSize, img.data());
    cf.status.updateImage = false;
}

void runFractalKernelAsync(clFractal& cf, clCore& cc, clLayerState& ls)
{
    while (cf.image.current_sample_count < cf.image.next_update_sample_count && !cf.stop)
    {
        cl_int err = 0;
        cl_int3 sampling_info = {
            cf.image.current_sample_count,
            std::min<int>(cf.image.current_sample_count + cf.samples_per_kernel_run, cf.image.target_sample_count),
            cf.image.target_sample_count
        };
        err = cc.setKernelArg(ls.fractalKernel.kernel, static_cast<int>(FractalKernelArg::SAMPLING_INFO), sampling_info, "sampling_info", cf.verbosity);
        cc.runFractalKernel(ls, cf);
        cf.image.current_sample_count = sampling_info.y;
    }
    cf.status.kernelRunning = false;
}

void runImgKernelAsync(clFractal& cf, clCore& cc, clLayerState& ls)
{
    cl_int err = 0;
    cl_int3 sampling_info = {
        cf.image.current_sample_count,
        std::min<int>(cf.image.current_sample_count + cf.samples_per_kernel_run, cf.image.target_sample_count),
        cf.image.target_sample_count
    };
    const uint32_t maxVal = sampling_info.y * cf.maxIter;
    err = cc.setKernelArg(ls.imgKernel.kernel, 1, maxVal, "histogram theoretical max", cf.verbosity);
    err = cc.setKernelArg(ls.imgKernel.kernel, 3, sampling_info, "sampling info", cf.verbosity);
    cc.setImgKernelArguments(ls, cf);
    cc.runImgKernel(ls, cf);
    cf.status.done = cf.image.current_sample_count == cf.image.target_sample_count;
    cf.status.imgKernelRunning = false;
    cf.status.updateImage = true;
}
