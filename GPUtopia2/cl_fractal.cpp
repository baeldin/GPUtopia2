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

clFractal::clFractal(const clFractalContainer& cfc)
{
	this->params = cfc.params;
	this->image = cfc.image;
	this->fractalCLFragmentFile = cfc.fractalCLfragmentFile;
	this->outsideColoringCLFragmentFile = cfc.outsideColoringCLfragmentFile;
	this->insideColoringCLFragmentFile = cfc.insideColoringCLfragmentFile;
	this->useDouble = cfc.useDouble;
	this->mode = cfc.mode;
	this->maxIter = cfc.maxIter;
	this->bailout = cfc.bailout;
	this->frs = cfc.frs;
	this->flamePointSelection = cfc.flamePointSelection;
	this->flameWarmup = cfc.flameWarmup;
	this->gradient = Gradient(cfc.gradCont);
}