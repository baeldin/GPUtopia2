__parameters:
float parameter colorSpeed = 0.1f;
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
		float c = 0.5 + 0.5 * sin(@colorSpeed * (d + atan2(z.y, z.x)));
		const int ci = (int)(255.f * fsRGBtoLinear(c));
		int4 outCol = (int4)(ci, ci, ci, 255);
		atomic_fetch_add(&colorsR[i], outCol.x); // , memory_order_relaxed, memory_scope_device);
		atomic_fetch_add(&colorsG[i], outCol.y); //, memory_order_relaxed);
		atomic_fetch_add(&colorsB[i], outCol.z); // , memory_order_relaxed);
		atomic_fetch_add(&colorsA[i], outCol.w); // , memory_order_relaxed);
__functions:
