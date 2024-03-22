__parameters:
float parameter colorDensity = 1.f;
__init:
//=====| coloring init
		float2 z_old = z;
		float d = 0.f;
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
		colors[i] += getColor(gradient, @colorDensity * d, nColors);
		