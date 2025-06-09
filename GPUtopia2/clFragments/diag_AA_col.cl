__parameters:
int parameter diag_sample_count = 55;
enum parameter pattern = 0;
Lattice Tent Disc
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
	real2 sample_position = 0.;
	if (@pattern == 0)
		sample_position = R2_offset(1, sample) - 0.5f;
	else if (@pattern == 1)
		sample_position = tent(R2_offset(1, sample));
	else if (@pattern == 2)
		sample_position = disc(R2_offset(1, sample));
	if (dot(z - sample_position, z - sample_position) < dot_dist)
	{
		outColor = (int4)(150, 0, 0, 255);
	}
}
setColor(colorsRGBA, outColor, pixelIdx);
__functions:
// void