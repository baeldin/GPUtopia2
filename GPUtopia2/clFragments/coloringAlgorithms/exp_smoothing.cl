__parameters:
float parameter colorDensity = 1.f;
__init:
//=====| coloring init 
	    real sum1 = 0.f;
	    real sum2 = 0.f;
	    complex z_old = (complex)(0., 0.);
__loop:
//=========| coloring loop
            sum1 += exp(1.f/length(z-z_old));
            sum2 += exp(-length(z));
            z_old = z;
__final:
//=====| coloring final
// 		// this part has to set colors[i], the components have
// 		// to be in [0., 1.]
//
		int4 outColor = {1, 0, 0, 1};
		if (bailed_out(z, bailout)) 
		{
			outColor = getColor(gradient, @colorDensity * 0.01 * sum2, nColors);
		} 
		else 
		{
			outColor = getColor(gradient, @colorDensity * 0.01 * sum1, nColors);
		}
		setColor(colorsRGBA, outColor, pixelIdx);
__functions:
// void