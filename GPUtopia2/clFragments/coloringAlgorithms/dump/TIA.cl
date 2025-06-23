__parameters:
  real parameter power = 1.f;
  real parameter bailout = 1e20f;
  real parameter colorDensity = 1.f;
__init:
//=====| coloring init 
  real sum = 0.0;
  real sum2 = 0.0;
  real ac = abs(z0);
  real il = 1/log(@power);
  real lp = log(log(@bailout)/2.0);
  real az2 = 0.0;
  real lowbound = 0.0;
  real f = 0.0;
  int first = 1;
  real index = 0;
__loop:
//=========| coloring loop
  sum2 = sum;
  if (first == 0) 
  {
    az2 = abs(z - z0);
    lowbound = fabs(az2 - ac);
    sum = sum + ((abs(z) - lowbound) / (az2+ac - lowbound));
  }
  else 
  {
    first = 0;
  }
__final:
//=====| coloring final
// 		// this part has to use setColors(), the components have
// 		// to be in [0., 1.]
//
  sum = sum / (iter);
  sum2 = sum2 / (iter-1);
  f = il*lp - il*log(log(abs(z)));
  index = sum2 + (sum-sum2) * (f+1);
  outColor += getColor(gradient, @colorDensity * 0.01f * index, nColors);
		// setColor(colorsRGBA, outColor, pixelIdx);
__functions:
// void

/* init:
  float sum = 0.0
  float sum2 = 0.0
  float ac = abs(#pixel)
  float il = 1/log(@power)
  float lp = log(log(@bailout)/2.0)
  float az2 = 0.0
  float lowbound = 0.0
  float f = 0.0
  BOOL first = true
loop:
  sum2 = sum
  IF (!first)
    az2 = cabs(#z - #pixel)
    lowbound = abs(az2 - ac)
    sum = sum + ((cabs(#z) - lowbound) / (az2+ac - lowbound))
  ELSE
    first = false
  ENDIF
final:
  sum = sum / (#numiter)
  sum2 = sum2 / (#numiter-1)
  f = il*lp - il*log(log(cabs(#z)))
  #index = sum2 + (sum-sum2) * (f+1)  */