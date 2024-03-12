__parameters:
float parameter colorSpeed = 0.1f;
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
		float c = 0.5 + 0.5 * sin(@colorSpeed * (d + atan2(z.y, z.x)));
		// float c = 0.5f + 0.5f * sin(length(z));
		c = fsRGBtoLinear(c);
		colors[i].xyzw += (float4)(c, c, c, 1.f);