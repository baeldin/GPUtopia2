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

bool applyNavigationDelta(clFractalImage& dst, const clFractalImage& from,
    const clFractalImage& to, const bool doCenter, const bool doZoom, const bool doAngle)
{
    // zoom <= 0 is never a real view; layers::beginFrame parks cf_old.image.zoom
    // at -1 as a startup sentinel and the ratio below would come out negative.
    if (from.zoom <= 0. || to.zoom <= 0. || dst.zoom <= 0.) return false;
    // The navigation step as a relative change. Adopting `to` outright would
    // snap dst onto it and discard whatever offset dst was given.
    const Complex<double> centerDelta = to.center - from.center;
    const double zoomRatio = to.zoom / from.zoom;
    const float angleDelta = to.angle - from.angle;
    const bool movesCenter = doCenter && (centerDelta.x != 0. || centerDelta.y != 0.);
    const bool movesZoom = doZoom && zoomRatio != 1.;
    const bool movesAngle = doAngle && angleDelta != 0.f;
    if (!movesCenter && !movesZoom && !movesAngle) return false;
    // Order matters: the centre shift is expressed in the frame dst had before
    // this step, so it has to be applied while dst.zoom and dst.angle still
    // hold their old values.
    if (movesCenter)
    {
        // image2complex maps a pixel to rotation() * (relative * span) + centre,
        // and span is 4/zoom. So one drag is the same movement on screen but a
        // different complex offset per layer: scaled by the ratio of the zooms,
        // rotated by however much the two orientations differ.
        const Complex<double> intoDstFrame =
            dst.rotation() * from.rotation().conj() * (from.zoom / dst.zoom);
        dst.center = dst.center + centerDelta * intoDstFrame;
    }
    if (movesZoom) dst.zoom *= zoomRatio;
    if (movesAngle) dst.angle += angleDelta;
    dst.updateComplexSubplane();
    return true;
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