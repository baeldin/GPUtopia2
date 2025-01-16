#include "cl_fractal.h"

// update coordinates within the complex plane if zoom or aspect ratio is changed
void clFractalImage::updateComplexSubplane()
{
    this->aspectRatio = (float)size.x / (float)size.y;
    this->span = { 4.f / zoom, 4.f / zoom / aspectRatio };
}

Complex<float> clFractalImage::image2complex(const cl_float2 xy) const
{
    const Complex<float> relativeImgCoord = Complex<float>(
        xy.x / (float)this->size.x - 0.5, 
        xy.y / (float)this->size.y - 0.5);
    return relativeImgCoord * this->rotation + this->center;
}