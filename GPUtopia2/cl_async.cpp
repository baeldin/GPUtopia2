#include "cl_async.h"

void asyncOpenCL::clShepherd(const clFractal& cf, const clCore& co)
{
    if (clIsRunning)
    {
        haltExecution = true;
    }
    cl_int localSize = WG_SIZE;
    cl_int globalSize = ceil(
        cf.image.size.x * cf.image.size.y / (float)localSize) * localSize;
    cl_int err = CL_SUCCESS;
    cl_int chunkSize = 2048;
    while (!haltExecution && chunkSize < chunkSize)
    {
        cl::NDRange globalRange = cl::NDRange(globalSize);
        cl::NDRange localRange = cl::NDRange(localSize);
        err = co.queue.enqueueNDRangeKernel(co.kernel, cl::NullRange, globalRange, localRange);
        if (err != CL_SUCCESS) {
            std::cerr << "Failed to enqueue kernel. Error code: " << err << std::endl;
        }
        co.queue.finish();
        chunkSize += std::max(2048, globalSize);
    }
}