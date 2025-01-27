__parameters:
float parameter colorDensity = 1.f;
__init:
//=====| coloring init 
__loop:
//=========| coloring loop
			// empty for smooth mandelbrot
__final:
//=====| coloring final
// 		// this part has to set colors[i], the components have
// 		// to be in [0., 1.]
//
		//colors[i].xyzw += getColor(gradient, @colorDensity * 0.01 * iter, nColors);
		int4 outColor = getColor(gradient, @colorDensity * 0.01 * iter, nColors);
		atomic_fetch_add(&colorsRGBA[4 * i    ], outColor.x); // , memory_order_relaxed, memory_scope_device);
		atomic_fetch_add(&colorsRGBA[4 * i + 1], outColor.y); // , memory_order_relaxed, memory_scope_device);
		atomic_fetch_add(&colorsRGBA[4 * i + 2], outColor.z); // , memory_order_relaxed, memory_scope_device);
		atomic_fetch_add(&colorsRGBA[4 * i + 3], outColor.w); // , memory_order_relaxed, memory_scope_device);
__functions:
// void