__parameters:
    complex parameter Weight1 = 1.;
    complex parameter Freq1 = 1.;
    complex parameter Weight2 = 0.01;
    complex parameter Freq2 = 1.;
__init:
//=====| fractal formula init
        complex z = z0;
        real w = 0.5 + 0.5 * sin(0.1 * z0.x);
__loop:
//=========| fractal formula loop
            complex v1x = (w * @Weight1) * stream_func_gradient_X(@Freq1 * z.x, @Freq1 * z.y);
            complex v1y = (w * @Weight1) * stream_func_gradient_Y(@Freq1 * z.x, @Freq1 * z.y);
            complex v2x = (((real)1. - w) * @Weight2) * stream_func_gradient_X(@Freq2 * z.x, @Freq2 * z.y);
            complex v2y = (((real)1. - w) * @Weight2) * stream_func_gradient_Y(@Freq2 * z.x, @Freq2 * z.y);
            complex v1 = (complex)(v1x.x + v1y.y, v1x.y - v1y.x);
            complex v2 = (complex)(v2x.x + v2y.y, v2x.y - v2y.x);
            z += v1 + v2;
__bailout:
//=| factal bailout function
    // must always define bool bailedout!!!
    bool bailedout = (dot(z, z) > bailout*bailout);
__functions:
//=| functions which are usable in the init, loop, and bailout parts
complex stream_func_gradient_X(const complex xold, const complex yold)
{
    real a = 1. / sqrt(3.);
    complex f = csin(((real)2. * a) * xold);
    complex g = csin(a * xold + yold);
    complex h = csin(a * xold - yold);
    complex gy = ccos(a * xold + yold);
    complex hy = -ccos(a * xold - yold);
    return f*gy*h + f*g*hy;
}
complex stream_func_gradient_Y(const complex xold, const complex yold)
{
    real a = 1. / sqrt(3.);
    complex f = csin((real)2. * a * xold);
    complex g = csin(a * xold + yold);
    complex h = csin(a * xold - yold);
    complex fx = (real)2. * a * ccos((real)2. * a * xold);
    complex gx = a * ccos(a * xold + yold);
    complex hx = a * ccos(a * xold - yold);
    return -(fx*g*h + f*gx*h + f*g*hx);
}
complex stream_func_gradient_X1(const complex xold, const complex yold)
{
    return csin(xold) * ccos(yold);
}
complex stream_func_gradient_Y1(const complex xold, const complex yold)
{
    return csin(yold) * ccos(xold);
}