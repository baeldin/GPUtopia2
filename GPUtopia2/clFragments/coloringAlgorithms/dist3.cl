__parameters:
float parameter colorSpeed = 0.1f;
__init:
//=====| coloring init
		float2 z_old = z;
		float d = 0.f;
__loop:
//=========| coloring loop
			// empty for smooth mandelbrot
			d += length(z - z_old) - length(z - z0);
			z_old = z;
__final:
//=====| coloring final
// 		// this part has to set colors[i], the components have
// 		// to be in [0., 1.)
//
		float c = fround(d - floor(d));
		// float c = 0.5f + 0.5f * sin(length(z));
		c = fsRGBtoLinear(c);
		colors[i].xyzw += (float4)(c, c, c, 1.f);