__parameters:
float parameter colorDensity = 1.f;
real parameter power = 2.;
enum parameter repeat = 1;
No, Yes
__init:
//=====| coloring init
		complex dz = (0, 0);
__loop:
//=========| coloring loop
			// empty for smooth mandelbrot
			dz = cmul(cpow(z, @power-1.), dz);
			dz = cmul(@power, dz) + (complex)(1., 0.);
__final:
//=====| coloring final
// 		// this part has to set colors[i], the components have
// 		// to be in [0., 1.)
		real c = @power * log(length(z)) * length(z);
		c = c / pow(length(dz), (real)1. / @power);
		if (@repeat == 0)
		    c = c > (real)1. ? (real)1. : c;
		outColor += getColor(gradient, @colorDensity * c, nColors);
		// setColor(colorsRGBA, outColor, pixelIdx);
__functions:
//=====| functions for use in the init, loop, and final parts

/*UF CODE OF DISTANCE ESTIMATOR:
init:
  complex dz = (0,0)
loop:
  dz = @power * #z^(@power-1) * dz + 1
final:
  #index = (@power*log(cabs(#z)) * cabs(#z) / cabs(dz))^(1/@power)*/