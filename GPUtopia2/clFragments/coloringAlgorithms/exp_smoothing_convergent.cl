__parameters:
float parameter colorDensity = 1.f;
__init:
//=====| coloring init 
	    float sum = 0.f;
	    complex zold = (complex)(0., 0.);
__loop:
//=========| coloring loop
            sum += fexp(1.f/abs(z-z_old));
            z_old = z;
__final:
//=====| coloring final
// 		// this part has to set colors[i], the components have
// 		// to be in [0., 1.]
//
		int4 outColor = getColor(gradient, @colorDensity * 0.01 * sum, nColors);
		atomic_fetch_add(&colorsR[i], outColor.x); // , memory_order_relaxed, memory_scope_device);
		atomic_fetch_add(&colorsG[i], outColor.y); //, memory_order_relaxed);
		atomic_fetch_add(&colorsB[i], outColor.z); // , memory_order_relaxed);
		atomic_fetch_add(&colorsA[i], outColor.w); // , memory_order_relaxed);
__functions:
// void