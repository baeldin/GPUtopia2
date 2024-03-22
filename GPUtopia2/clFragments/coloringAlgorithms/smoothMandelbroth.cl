__parameters:
float parameter colorDensity = 1.f;
float parameter bailout = 128.f;
float parameter power = 2.f;
__init:
//=====| coloring init
		float il = 1.f / log(@power);  // Inverse log(power).
		float lp = log(log(@bailout)); // log(log bailout).
__loop:
//=========| coloring loop
			// empty for smooth mandelbrot
__final:
//=====| coloring final
// 		// this part has to set colors[i], the components have
// 		// to be in [0., 1.)
//
		float c = 0.01f * ((float)iter + il * lp - il * log(0.5f * log(dot(z, z))));

		// c = sqrt(c);
		colors[i].xyzw += getColor(gradient, @colorDensity * c, nColors);