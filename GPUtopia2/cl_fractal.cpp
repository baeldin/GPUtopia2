#include "cl_fractal.h"

clFractalMinimal::clFractalMinimal(const clFractal* cf)
{
    centerX = cf->image.center.x;
    centerY = cf->image.center.y;
    zoom = cf->image.zoom;
    angle = cf->image.angle;
    sizeX = cf->image.size.x;
    sizeY = cf->image.size.y;
    quality = cf->image.targetQuality;
    brightness = cf->flameRenderSettings.x;
    gamma = cf->flameRenderSettings.y;
    vibrancy = cf->flameRenderSettings.z;
    maxIter = cf->maxIter;
    mode = cf->mode;
    pointSelection = cf->flamePointSelection;
    fractalIntParameters = cf->params.fractalParameterMaps.integerParameters;
    fractalFloatParameters = cf->params.fractalParameterMaps.floatParameters;
    fractalBoolParameters = cf->params.fractalParameterMaps.boolParameters;
    coloringIntParameters = cf->params.coloringParameterMaps.integerParameters;
    coloringFloatParameters = cf->params.coloringParameterMaps.floatParameters;
    coloringBoolParameters = cf->params.coloringParameterMaps.boolParameters;
    for (int ii = 0; ii < cf->gradient.nodeCount; ii++)
    {
        const int intColor = cf->gradient.nodeColors[ii].toUFint();
        const int idx = cf->gradient.nodeLocation[ii];
        this->gradientColors.push_back(std::pair<int, int>(idx, intColor));
        this->gradientFillOrder.push_back(cf->gradient.fillOrder[ii]);
    }
    fractalCLFragmentFile = cf->fractalCLFragmentFile;
    coloringCLFragmentFile = cf->coloringCLFragmentFile;
}

clFractal::clFractal(const clFractalMinimal& cfm)
{
    this->image.center.x = cfm.centerX;
    this->image.center.y = cfm.centerY;
    this->image.zoom = cfm.zoom;
    this->image.angle = cfm.angle;
    this->image.size.x = cfm.sizeX;
    this->image.size.y = cfm.sizeY;
    this->image.targetQuality = cfm.quality;
    this->flameRenderSettings.x = cfm.brightness;
    this->flameRenderSettings.y = cfm.gamma;
    this->flameRenderSettings.z = cfm.vibrancy;
    this->maxIter = cfm.maxIter;
    this->mode = cfm.mode;
    this->flamePointSelection = cfm.pointSelection;
    this->params.fractalParameterMaps.integerParameters = cfm.fractalIntParameters;
    this->params.fractalParameterMaps.floatParameters = cfm.fractalFloatParameters;
    this->params.fractalParameterMaps.boolParameters = cfm.fractalBoolParameters;
    this->params.coloringParameterMaps.integerParameters = cfm.coloringIntParameters;
    this->params.coloringParameterMaps.floatParameters = cfm.coloringFloatParameters;
    this->params.coloringParameterMaps.boolParameters = cfm.coloringBoolParameters;
    std::vector<color> nodeColors;
    std::vector<int> nodeLocations;
    std::vector<int> fillOrder;
    for (int ii = 0; ii < cfm.gradientColors.size(); ii++)
    {
        const uint32_t ci = cfm.gradientColors[ii].second;
        const color c(ci);
        nodeColors.push_back(c);
        nodeLocations.push_back(cfm.gradientColors[ii].first);
    }
    this->gradient = Gradient(cfm.gradientColors.size(), nodeColors, nodeLocations, cfm.gradientFillOrder);
}

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