__parameters:
float parameter colorDensity = 1.f;
float parameter lengthWeight = 0.1f;
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
		// float c = d - floor(d);
		colors[i].xyzw += getColor(gradient, @colorDensity * (1.f + @lengthWeight * length(z0))*length(z-z0), nColors);
		// float c = 0.5f + 0.5f * sin(length(z));
		// colors[i].xyzw += (float4)(c, c, c, 1.f);