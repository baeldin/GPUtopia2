__parameters:
float parameter colorDensity = 1.f;
__init:
//=====| coloring init
        float min_distance = 1.;
        float d = 1e10;
        complex z_old = {0., 0.};
__loop:
//=========| coloring loop
			// empty for smooth mandelbrot
			d = length(z - z_old);
			if (d < min_distance)
			{
				min_distance = d;
			}
			int2 xy = revert_complex_coordinates(z, image_size, complex_subplane);
			if (xy.x < image_size.x && xy.x >= 0 && xy.y < image_size.y && xy.y >= 0)
			{
				
				int4 color = getColor(gradient, @colorDensity * min_distance, nColors);
				int pixelIndex = xy.y * image_size.x + xy.x;
				int4 col = getColor(gradient, @colorDensity * min_distance, nColors);
				// colors[pixelIndex] += (int4)(col.x, col.y, col.z, 256); // color;
				// atomic_fetch_add_explicit(&hitCounts[hitY * width + hitX], 1, memory_order_relaxed);
				atomic_fetch_add(&colorsR[pixelIndex], col.x); // , memory_order_relaxed, memory_scope_device);
				atomic_fetch_add(&colorsG[pixelIndex], col.y); //, memory_order_relaxed);
				atomic_fetch_add(&colorsB[pixelIndex], col.z); // , memory_order_relaxed);
				atomic_fetch_add(&colorsA[pixelIndex], col.w); // , memory_order_relaxed);
			}
			z_old = z;
			//int xx = 500 * z.x;
			//int yy = 500 * z.y;
__final:
//=====| coloring final
// 		// this part has to set colors[i], the components have
// 		// to be in [0., 1.)
//
		// float c = 0.01f * ((float)iter + il * lp - il * log(0.5f * log(dot(z, z))));

		// c = sqrt(c);
		// colors[i].xyzw += getColor(gradient, @colorDensity * c, nColors);
__functions:
// empty
