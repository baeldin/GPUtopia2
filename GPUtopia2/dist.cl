__parameters:
float parameter bailout = 128.f;
float parameter power = 2.f;
__init:
//=====| coloring init
		float il = 1.f / log(@power);  // Inverse log(power).
		float lp = log(log(@bailout)); // log(log bailout).
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
		//float c = 0.02f * ((float)iter + il * lp - il * log(0.5f * log(dot(z, z))));
		float c = 0.5f + 0.5f * sin(0.1f * d);
		colors[i].xyzw += (float4)(c, c, c, 1.f);