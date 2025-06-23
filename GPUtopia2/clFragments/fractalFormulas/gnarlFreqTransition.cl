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
            complex v1 = (complex)(v1x.x + v1y.y, v1x.y + v1y.x);
            complex v2 = (complex)(v2x.x + v2y.y, v2x.y + v2y.x);
            z += v1 + v2;
__bailout:
//=| factal bailout function
    // must always define bailedOut!!!
    bailedOut = (dot(z, z) > bailout*bailout);
__functions:
//=| functions which are usable in the init, loop, and bailout parts
complex stream_func_gradient_X(const complex x, const complex y)
{
    real a = 1. / sqrt(3.);
    complex f = csin(((real)2. * a) * x);
    complex g = csin(a * x + y);
    complex h = csin(a * x - y);
    complex gy = ccos(a * x + y);
    complex hy = -ccos(a * x - y);
    return f*gy*h + f*g*hy;
}
complex stream_func_gradient_Y(const complex x, const complex y)
{
    real a = 1. / sqrt(3.);
    complex f = csin((real)2. * a * x);
    complex g = csin(a * x + y);
    complex h = csin(a * x - y);
    complex fx = (real)2. * a * ccos((real)2. * a * x);
    complex gx = a * ccos(a * x + y);
    complex hx = a * ccos(a * x - y);
    return -(fx*g*h + f*gx*h + f*g*hx);
}
complex stream_func_gradient_X1(const complex x, const complex y)
{
    return csin(x) * ccos(y);
}
complex stream_func_gradient_Y1(const complex x, const complex y)
{
    return csin(y) * ccos(x);
}