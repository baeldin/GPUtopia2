__parameters:
float parameter colorDensity = 1.f;
__init:
//=====| coloring init
        real min_distance = 1e20;
	real d;
__loop:
//=========| coloring loop
			// empty for smooth mandelbrot
			d = length(z - round(z));
			if (d < min_distance)
			{
				min_distance = d;
			}
			
__final:
//=====| coloring final
// 		// this part has to set colors[i], the components have
// 		// to be in [0., 1.)
//
		// float c = 0.01f * ((float)iter + il * lp - il * log(0.5f * log(dot(z, z))));

		// c = sqrt(c);
		int4 outColor = getColor(gradient, @colorDensity * min_distance, nColors);
		setColor(colorsRGBA, outColor, pixelIdx);
__functions:
// empty
