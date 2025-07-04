//@__DOUBLE
//@__MODE
#ifdef USE_DOUBLE
typedef double real;
typedef double2 real2;
typedef double4 real4;
#else
typedef float real;
typedef float2 real2;
typedef float4 real4;
#endif

//@__COLORING
//@__AA
//@__COMPLEX

//@__formulaFunctions
//@__coloringInsideFunctions
//@__coloringOutsideFunctions

complex get_complex_coordinates(const real2 xy, const int2 img_size, const real4 cz, const real2 rot)
{
    complex z = (complex)(
        (xy.x / (real)img_size.x - 0.5f) * cz.z, 
        (xy.y / (real)img_size.y - 0.5f) * cz.w);
    z = cmul(z, rot);
    return z + cz.xy;
}

int2 revert_complex_coordinates(const real2 z, const int2 img_size, const real4 cz, const real2 rot)
{
    complex z_tmp = z - cz.xy;
    z_tmp = cmul(z_tmp, conj(rot));
    return (int2)(
        floor((z_tmp.x / cz.z + 0.5f) * (img_size.x - 1)),
        floor((z_tmp.y / cz.w + 0.5f) * (img_size.y - 1)));
}

__kernel void computeLoop(
    const int2 image_size,             // 0:  image sizeX, image sizeY
    const real4 complex_subplane,      // 1:  {centerX, centerY, width, width / aspectRatio}
    const real2 rot,                   // 2:  complex rotation
    const int3 sampling,               // 3:  {sampleStart, sampleEnd, nSamplesTotal}
    const int maxIterations,           // 4:  maxIterations
    const float bailout,               // 5:  bailout value
    const int nColors,                 // 6:  colors in gradient
    __global const float4* gradient,   // 7:  gradient
#ifdef FLAME
    __global atomic_uint* colorsRGBA,  // 8:  output R
#else
    __global uint* colorsRGBA,  // 8:  output R
#endif
    const int flamePointSelection,     // 9: discard points, used for flame
    const int flameWarmup,             // 10: warmup before splatting flame samples
    const int mode,                    // 11: mode (0 = escape time, 1 = flame)
//@__kernelArguments)
{
    // Get Parallel Index
    unsigned int i = get_global_id(0);
    const int pixelIdx = i;
    const int y = (image_size.y - 1) - pixelIdx / image_size.x;
    const int x = pixelIdx - image_size.x * (image_size.y - y - 1);
    const int sample_count = sampling.y - sampling.x;
    const int offset_fac = min(1, sampling.z - 1);
    bool use_point = true;
    // check if point is to be used
    {
        int4 outColor = { 0, 0, 0, 0 };
        for (int s = sampling.x; s < sampling.y; s++)
        {
            const real2 R2 = R2_offset(pixelIdx, s);
            if (flamePointSelection > 0)
            {
                int iter = 0;
                real2 sample_position = (real2)(x, y) + offset_fac * (real2)(
                    tent(R2.x),
                    tent(R2.y));
                const real2 z0 = get_complex_coordinates(sample_position, image_size, complex_subplane, rot);
                //@__formulaInit
                bool bailedOut = false;
                while (!bailedOut && iter < maxIterations)
                {
                    //@__formulaLoop
                    //@__bailout
                    iter++;
                }
                if (bailedOut)
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
                real2 sample_position = (real2)(x, y) + offset_fac * (real2)(
                    tent(R2.x),
                    tent(R2.y));
                const real2 z0 = get_complex_coordinates(sample_position, image_size, complex_subplane, rot);
                //@__formulaInit
                //@__coloringInsideInit
                //@__coloringOutsideInit

                bool bailedOut = false;
                while (!bailedOut && iter < maxIterations)
                {
                    //@__formulaLoop
                    //@__coloringInsideLoop
                    //@__coloringOutsideLoop
                    //@__bailout
                    iter++;
                }
                if (bailedOut)
                {
                    //@__coloringOutsideFinal
		        } 
                else
                {
                    //@__coloringInsideFinal
		        }
            }
        }
        setColor(colorsRGBA, outColor, pixelIdx);
    }
}


