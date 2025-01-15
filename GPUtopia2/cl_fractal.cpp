#include "cl_fractal.h"

// update coordinates within the complex plane if zoom or aspect ratio is changed
void clFractalImage::updateComplexSubplane()
{
    complexSubplane.z = 4.f / zoom;
    aspectRatio = (float)size.x / (float)size.y;
    complexSubplane.w = complexSubplane.z / aspectRatio;
}

Complex<float> clFractalImage::image2complex(const cl_float2 xy) const
{
    const Complex<float> center = Complex<float>(this->complexSubplane.x, this->complexSubplane.y);
    const Complex<float> span = Complex<float>(0.5f * this->complexSubplane.z, 0.5f * this->complexSubplane.w);
    const cl_float2 relativeImgCoord = { 2.f * xy.x / (float)this->size.x - 1.f, 2.f * xy.y / (float)this->size.y - 1.f };
    return Complex<float>(
        center.x + relativeImgCoord.x * span.x,
        center.y + relativeImgCoord.y * span.y);
}