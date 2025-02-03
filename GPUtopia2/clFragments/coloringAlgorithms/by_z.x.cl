__parameters:
float parameter colorDensity = 1.f;
__init:
//=====| coloring init
__loop:
//=========| coloring loop
			// empty for smooth mandelbrot
__final:
//=====| coloring final
// 		// this part has to set colors[i], the components have
// 		// to be in [0., 1.)
//
		int4 outColor = getColor(gradient, @colorDensity * z.x, nColors);
		setColor(colorsRGBA, outColor, pixelIdx);
__functions:
//=====| functions for use in the init, loop, and final parts