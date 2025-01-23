#include "cl_fractal.h"

// update coordinates within the complex plane if zoom or aspect ratio is changed
void clFractalImage::updateComplexSubplane()
{
    this->aspectRatio = (double)size.x / (double)size.y;
    this->span = { 4. / zoom, 4. / zoom / aspectRatio };
}

Complex<double> clFractalImage::image2complex(const cl_double2 xy) const
{
    const Complex<double> relativeImgCoord = Complex<double>(
        (xy.x / (double)this->size.x - 0.5) * this->span.x, 
        ((this->size.y - xy.y) / (double)this->size.y - 0.5) * this->span.y);
    return relativeImgCoord * this->rotation() + this->center;
}