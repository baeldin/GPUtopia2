float clampZeroToOne(const float x)
{
    return fmax(0.f, fmin(1.f, x));
}


const float a = 1.f / 12.92f;
const float b = 1.f / 1.055f;
const float inv256 = 1.f / 256.f;

const float invGamma = 1.f / 2.4f;
float flinearToSRGB(const float x) {
    return (clampZeroToOne(x) <= 0.0031308f) ? clampZeroToOne(x) * 12.92f : 1.055f * pow(clampZeroToOne(x), invGamma) - 0.055f;
}

float4 linearToSRGB(const float4 v) {
    return (float4)(flinearToSRGB(v.x), flinearToSRGB(v.y), flinearToSRGB(v.z), flinearToSRGB(v.w));
}

//log density scaling
float4 logscale(float4 acc, float brightness, float max_density)
{
    float ls = log10(1.0 + brightness * acc.w / max_density) / acc.w;
    float4 lh = acc * ls;
    return lh;
}

__kernel void imgProcessing(
    __global const int* inColorsR,   // accumulated int R
    __global const int* inColorsG,   // accumulated int G
    __global const int* inColorsB,   // accumulated int B
    __global const int* inColorsA,   // accumulated int A
    const int inColorsMaxValues,     // max value per component (r, g, b, alpha)
    __global float4* outColors,      // output float4 colors
    const int3 sampling,             // sampling info
    const int mode,                  // mode (0 = escape time, 1 = flame), UNUSED
    const float brightness,          // flame brightness
    const float gamma,               // flame gamma
    const float vibrancy)            // flame vibrancy
{
    unsigned int i = get_global_id(0);

    // define some default values for now
    const float inv_gamma = 1.f / gamma;

    if (mode == 0)
    {
        // do ET stuff
        const float invFactor = 1.f / (float)(256 * sampling.z);
        outColors[i] = linearToSRGB((float4)(
            invFactor * (float)inColorsR[i],
            invFactor * (float)inColorsG[i],
            invFactor * (float)inColorsB[i],
            invFactor * (float)inColorsA[i]));
    }
    /*else if (mode == 1)
    {
        // IFSRenderer attempt - work in progress
        float4 fColor = (float4)((float)inColors[i].x / 256.f, (float)inColors[i].y / 256.f, (float)inColors[i].z / 256.f, (float)inColors[i].w / 256.f);
        fColor = logscale(fColor, brightness, (float)inColorsMaxValues);

        // SKIP GAMMA THRESHOLD STUFF FOR NOW
        // do tone mapping stuff for flames
        //float funcval = 0.0;
        //if (gamma_threshold != 0.0)
        //{
           // funcval = pow(gamma_threshold, inv_gamma - 1.f);
        //}
        // float alpha;
        //if (fp.w < gamma_threshold)
        //{
           // float frac = fp.w / gamma_threshold;
           // alpha = (1.f - frac) * fp.w * funcval + frac * pow(fp.w, inv_gamma);
        //}
        //else
        float alpha = pow(fColor.w, inv_gamma);

        float vls = vibrancy * alpha / fColor.w;
        alpha = clamp(alpha, 0.f, 1.f);

        float3 o = vls * fColor.rgb + (1.f - vibrancy) * pow(fColor.xyz, (float3)(inv_gamma));
        o = clamp(o, 0.f, 1.f);

        o += (1.f - alpha) * (float3)(0.f);
        o = clamp(o, 0.f, 1.f);

        outColors[i] = linearToSRGB(fColor); //(float4)(1., 0.5, 0., 1.); //(o.x, o.y, o.z, alpha);
    }
    else if (mode == 2) //Reinhard
    {
        outColors[i] = linearToSRGB((float4)(
            (brightness * (float)inColors[i].x) / (1.f + brightness * (float)inColors[i].x),
            (brightness * (float)inColors[i].y) / (1.f + brightness * (float)inColors[i].y),
            (brightness * (float)inColors[i].z) / (1.f + brightness * (float)inColors[i].z),
            (brightness * (float)inColors[i].a) / (1.f + brightness * (float)inColors[i].a)));

    }*/
    else if (mode == 3) // something something log
    {

        float4 tmpColor = inv256 * (float4)(
            (float)inColorsR[i],
            (float)inColorsG[i],
            (float)inColorsB[i],
            (float)inColorsA[i]);
        float ls = log10(1.f + brightness * tmpColor.w / (float)inColorsMaxValues) / (float)tmpColor.w;
        tmpColor = ls * tmpColor;

        tmpColor = pow(tmpColor, inv_gamma);
        

        float alpha = pow(tmpColor.w, inv_gamma);
        float lsv = vibrancy * alpha / tmpColor.w;
        tmpColor.xyz = lsv * tmpColor.xyz + (1.f - vibrancy) * pow(tmpColor.xyz, inv_gamma);
        tmpColor.xyz = (float3)(
            clampZeroToOne(tmpColor.x),
            clampZeroToOne(tmpColor.y),
            clampZeroToOne(tmpColor.z));
        outColors[i] = tmpColor;
    }

}