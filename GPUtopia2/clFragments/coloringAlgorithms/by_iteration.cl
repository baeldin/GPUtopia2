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
		colors[i].xyzw += getColor(gradient, @colorDensity * 0.01 * iter, nColors);