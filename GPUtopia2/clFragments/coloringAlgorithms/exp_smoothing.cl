__parameters:
float parameter colorDensity = 1.f;
__init:
//=====| coloring init 
	    float sum = 0.f;
	    float sum2 = 0.f;
	    complex z_old = (complex)(0., 0.);
__loop:
//=========| coloring loop
            sum += exp(1.f/abs(z-z_old));
            sum2 += exp(-abs(z));
            z_old = z;
__final:
//=====| coloring final
// 		// this part has to set colors[i], the components have
// 		// to be in [0., 1.]
//
		int4 outColor = {0, 0, 0, 0};
		if (bailed_out(z, bailout)) 
		{
			outColor = getColor(gradient, @colorDensity * 0.01 * sum2, nColors);
		} 
		else 
		{
			outColor = getColor(gradient, @colorDensity * 0.01 * sum, nColors);
		}
		atomic_fetch_add(&colorsR[i], outColor.x); // , memory_order_relaxed, memory_scope_device);
		atomic_fetch_add(&colorsG[i], outColor.y); //, memory_order_relaxed);
		atomic_fetch_add(&colorsB[i], outColor.z); // , memory_order_relaxed);
		atomic_fetch_add(&colorsA[i], outColor.w); // , memory_order_relaxed);
__functions:
// void