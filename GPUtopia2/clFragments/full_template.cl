//@__COLORING
//@__AA
//@__COMPLEX
//@__formulaFunctions
//@__coloringFunctions

float2 get_complex_coordinates(const float2 xy, const int2 img_size, const float4 cz)
{
    return (float2)(
        cz.x + (xy.x / (float)img_size.x - 0.5f) * cz.z,
        cz.y + (xy.y / (float)img_size.y - 0.5f) * cz.w);
}

int2 revert_complex_coordinates(const float2 z, const int2 img_size, const float4 cz)
{
    return (int2)(
        floor(((z.x - cz.x) / cz.z + 0.5f) * (img_size.x - 1)),
        floor(((z.y - cz.y) / cz.w + 0.5f) * (img_size.y - 1)));
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
    const int3 sampling,               // 4:  {sampleStart, sampleEnd, nSamplesTotal}
    const int maxIterations,           // 5:  maxIterations
    const float bailout,               // 6:  bailout value
    const int nColors,                 // 7:  colors in gradient
    __global const float4* gradient,   // 8:  gradient
    __global atomic_uint* colorsR,     // 9:  output R
    __global atomic_uint* colorsG,     // 10: output G
    __global atomic_uint* colorsB,     // 11: output B
    __global atomic_uint* colorsA,     // 12: output A
    const int flamePointSelection,     // 13: discard points, used for flame
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
            if (flamePointSelection > 0)
            {
                int iter = 0;
                float2 sample_position = (float2)(x, y) + offset_fac * (float2)(
                    tent(fracf(fracf((float)s * inv_phi2A) + tofloat(lowbias32((uint)(2 * pixelIdx))))),
                    tent(fracf(fracf((float)s * inv_phi2B) + tofloat(lowbias32((uint)(2 * pixelIdx + 1))))));
                //float2 sample_position = (float2)(x, y) + offset_fac * (float2)(
                //    tent(fracf((float)s * phi + tofloat(lowbias32((uint)(2 * pixelIdx))))),
                //    tent(fracf((float)s / (float)sampling.z + tofloat(lowbias32((uint)(2 * pixelIdx + 1))))));
                const float2 z0 = get_complex_coordinates(sample_position, image_size, complex_subplane);
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
                    tent(fracf(fracf((float)s * inv_phi2A) + tofloat(lowbias32((uint)(2 * pixelIdx))))),
                    tent(fracf(fracf((float)s * inv_phi2B) + tofloat(lowbias32((uint)(2 * pixelIdx + 1))))));
                const float2 z0 = get_complex_coordinates(sample_position, image_size, complex_subplane);
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
        // colors[i].xyzw /= (float4)(sample_count, sample_count, sample_count, 1.f);
        // colors[i] = linearToSRGB(colors[i]);
        //colors[i].xyzw = (float4)((float)sampling.x, (float)sampling.y, (float)sampling.z, 1.);
    }
}


