__parameters:
float parameter colorDensity = 1.f;
float parameter lengthWeight = 0.1f;
__init:
//=====| coloring init 

		flame = true;
__loop:
//=========| coloring loop
			// empty for smooth mandelbrot
			// atomicAdd_g_f(volatile __global float* addr, float val)
			// revert_complex_coordinates(const float2 z, const int2 img_size, const float4 cz)
			int2 ic = revert_complex_coordinates(z, image_size, complex_subplane);
			if (ic.x >= 0 && ic.x < image_size.x && ic.y >= 0 && ic.y < image_size.y)
			{
				colors[i] += (float4)(@colorDensity, @colorDensity, @colorDensity, 1.f);
			}
__final:
//=====| coloring final
// 		// this part has to set colors[i], the components have
// 		// to be in [0., 1.)
