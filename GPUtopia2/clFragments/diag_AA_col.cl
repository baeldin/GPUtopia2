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
	real2 sample_position = 0.f;
	sample_position = R2_offset(1, sample) - 0.5f;
	if (dot(z - sample_position, z - sample_position) < dot_dist)
	{
		outColor = (int4)(150, 0, 0, 255);
	}
}
setColor(colorsRGBA, outColor, pixelIdx);
__functions:
// void