#define USE_DOUBLE 1
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
//@__coloringFunctions

complex get_complex_coordinates(const real2 xy, const int2 img_size, const float4 cz_in, const float2 rot_in)
{
    const real4 cz = { (real)cz_in.x, (real)cz_in.y, (real)cz_in.z, (real)cz_in.w };
    const complex rot = { (real)rot_in.x, (real)rot_in.y };
    complex z = (complex)(
        (xy.x / (real)img_size.x - 0.5f) * cz.z, 
        (xy.y / (real)img_size.y - 0.5f) * cz.w);
    z = cmul(z, rot);
    return z + cz.xy;
}

int2 revert_complex_coordinates(const real2 z, const int2 img_size, const float4 cz_in, const float2 rot_in)
{
    const real4 cz = { (real)cz_in.x, (real)cz_in.y, (real)cz_in.z, (real)cz_in.w };
    const complex rot = { (real)rot_in.x, (real)rot_in.y };
    complex z_tmp = z - cz.xy;
    z_tmp = cmul(z_tmp, conj(rot));
    return (int2)(
        floor((z_tmp.x / cz.z + 0.5f) * (img_size.x - 1)),
        floor((z_tmp.y / cz.w + 0.5f) * (img_size.y - 1)));
}

bool bailed_out(const real2 z, const real bailout)
{
//@__bailout
    return bailedout;
}
__kernel void computeLoop(
    const int2 image_size,             // 0:  image sizeX, image sizeY
    const float4 complex_subplane,     // 1:  {centerX, centerY, width, width / aspectRatio}
    const float2 rot,                  // 2:  complex rotation
    const int3 sampling,               // 3:  {sampleStart, sampleEnd, nSamplesTotal}
    const int maxIterations,           // 4:  maxIterations
    const float bailout,               // 5:  bailout value
    const int nColors,                 // 6:  colors in gradient
    __global const float4* gradient,   // 7:  gradient
    __global atomic_uint* colorsR,     // 8:  output R
    __global atomic_uint* colorsG,     // 9: output G
    __global atomic_uint* colorsB,     // 10: output B
    __global atomic_uint* colorsA,     // 11: output A
    const int flamePointSelection,     // 12: discard points, used for flame
    const int flameWarmup,             // 13: warmup before splatting flame samples
    const int mode,                    // 14: mode (0 = escape time, 1 = flame)
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
                real2 sample_position = (real2)(x, y) + offset_fac * (real2)(
                    tent(R2.x),
                    tent(R2.y));
                const real2 z0 = get_complex_coordinates(sample_position, image_size, complex_subplane, rot);
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


