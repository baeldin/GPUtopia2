//@__COLORING
//@__AA
//@__COMPLEX
//@__formulaFunctions
//@__coloringFunctions

complex get_complex_coordinates(const float2 xy, const int2 img_size, const float4 cz, const complex rot)
{
    complex z = (complex)(
        (xy.x / (float)img_size.x - 0.5f) * cz.z, 
        (xy.y / (float)img_size.y - 0.5f) * cz.w);
    z = cmul(z, rot);
    return z + cz.xy;
}

int2 revert_complex_coordinates(const float2 z, const int2 img_size, const float4 cz, const complex rot)
{
    complex z_tmp = z - cz.xy;
    z_tmp = cmul(z_tmp, conj(rot));
    return (int2)(
        floor((z_tmp.x / cz.z + 0.5f) * (img_size.x - 1)),
        floor((z_tmp.y / cz.w + 0.5f) * (img_size.y - 1)));
}

bool bailed_out(const float2 z, const float bailout)
{
//@__bailout
    return bailedout;
}
__kernel void computeLoop(
    __global int const* xx,            // 0:  pixel x values
    __global int const* yy,            // 1:  pixel y values
    const int2 image_size,             // 2:  image sizeX, image sizeY
    const float4 complex_subplane,     // 3:  {centerX, centerY, width, width / aspectRatio}
    const float2 rot,                  // 4:  complex rotation
    const int3 sampling,               // 5:  {sampleStart, sampleEnd, nSamplesTotal}
    const int maxIterations,           // 6:  maxIterations
    const float bailout,               // 7:  bailout value
    const int nColors,                 // 8:  colors in gradient
    __global const float4* gradient,   // 9:  gradient
    __global atomic_uint* colorsR,     // 10:  output R
    __global atomic_uint* colorsG,     // 11: output G
    __global atomic_uint* colorsB,     // 12: output B
    __global atomic_uint* colorsA,     // 13: output A
    const int flamePointSelection,     // 14: discard points, used for flame
    const int flameWarmup,             // 15: warmup before splatting flame samples
    const int mode,                    // 16: mode (0 = escape time, 1 = flame)
//@__kernelArguments)
{
    // Get Parallel Index
    unsigned int i = get_global_id(0);
    const int x = xx[i]; // Real Component
    const int y = yy[i]; // Imaginary Component
    const int pixelIdx = image_size.x * y + x;
    const int sample_count = sampling.y - sampling.x;
    int offset_fac = min(1, sampling.z - 1);
    bool use_point = true;
    // check if point is to be used
    {
        for (int s = sampling.x; s < sampling.y; s++)
        {
            const float2 R2 = R2_offset(pixelIdx, s);
            if (flamePointSelection > 0)
            {
                int iter = 0;
                float2 sample_position = (float2)(x, y) + offset_fac * (float2)(
                    tent(R2.x),
                    tent(R2.y));
                const float2 z0 = get_complex_coordinates(sample_position, image_size, complex_subplane, rot);
                //@__formulaInit
                while (!bailed_out(z, bailout) && iter < maxIterations)
                {
                    //@__formulaLoop
                    iter++;
                }
                if (bailed_out(z, bailout))
                {
                    use_point = flamePointSelection == 1 ? true : false;
                }
                else
                {
                    use_point = flamePointSelection == 2 ? true : false;
                }
            }
            if (use_point)
            {
                int iter = 0;
                float2 sample_position = (float2)(x, y) + offset_fac * (float2)(
                    tent(R2.x),
                    tent(R2.y));
                const float2 z0 = get_complex_coordinates(sample_position, image_size, complex_subplane, rot);
                //@__formulaInit
                //@__coloringInit
                while (!bailed_out(z, bailout) && iter < maxIterations)
                {
                    //@__formulaLoop
                    //@__coloringLoop
                    iter++;
                }
                //@__coloringFinal
            }
        }
    }
}


