__parameters:
float parameter colorDensity = 1.f;
__init:
//=====| coloring init 
__loop:
//=========| coloring loop
__final:
//=====| coloring final
// 		// this part has to use setColors(), the components have
// 		// to be in [0., 1.]
//
		outColor += getColor(gradient, @colorDensity * 0.01 * iter, nColors);
		// setColor(colorsRGBA, outColor, pixelIdx);
__functions:
// void