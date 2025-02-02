__parameters:
float parameter colorDensity = 1.f;
__init:
//=====| coloring init
		complex z_old = z;
		real d = 0.f;
__loop:
//=========| coloring loop
			// empty for smooth mandelbrot
			d += length(z - z_old);
			z_old = z;
__final:
//=====| coloring final
// 		// this part has to set colors[i], the components have
// 		// to be in [0., 1.)
//
		int4 outColor = getColor(gradient, @colorDensity * d, nColors);
		setColor(colorsRGBA, outColor, pixelIdx);
__functions:
//=====| functions for use in the init, loop, and final parts