__parameters:
int parameter diag_sample_count = 55;
int parameter pattern = 0;
float parameter dot_size = 0.04f;
__init:

//=====| coloring init 
const int gaussIdx = floor(z.x + 10000) * 10000 + floor(z.y);
const float dot_dist = 0.25 * @dot_size * @dot_size;
__loop:
//=========| coloring loop
			// empty for smooth mandelbrot
__final:
//=====| coloring final
// 		// this part has to set colorsR/G/B/A[i], 
int4 outColor = 255;
for (int sample = 0; sample <= @diag_sample_count; sample++)
{
	float2 sample_position = 0.f;
	if (@pattern == 0)
	{
		sample_position = (float2)(
			-0.5f + fracf((float)sample * inv_phi2A),
			-0.5f + fracf((float)sample * inv_phi2B));
	}
	else if (@pattern == 1)
	{
		sample_position = R2_offset(1, sample) - 0.5f;
	}
	else if (@pattern == 2)
	{
		float2 sp = R2_offset_fine(1, sample);
		sample_position = (float2)((float)sp.x - 0.5f, (float)sp.y - 0.5f);
	}
	else if (@pattern == 3)
	{
		double2 sp = R2_offset_fine64(1, sample);
		sample_position = (float2)((float)sp.x - 0.5f, (float)sp.y - 0.5f);
	}
	if (dot(z - sample_position, z - sample_position) < dot_dist)
	{
		outColor = (int4)(150, 0, 0, 255);
	}
}
atomic_fetch_add(&colorsR[i], outColor.x); // , memory_order_relaxed, memory_scope_device);
atomic_fetch_add(&colorsG[i], outColor.y); //, memory_order_relaxed);
atomic_fetch_add(&colorsB[i], outColor.z); // , memory_order_relaxed);
atomic_fetch_add(&colorsA[i], outColor.w); // , memory_order_relaxed);
__functions:
// void