__parameters:
    float parameter c1 = 0.1f;
    float parameter f1 = 1.f
    float parameter c2 = 0.1f;
    float parameter f2 = 2.f;
    float parameter c3 = 0.1f;
    float parameter f3 = 3.f;
__init:
//=====| fractal formula init
        real2 z = z0;
__loop:
//=========| fractal formula loop
            z += (real2)(
        @c1 * stream_func_gradient_X(@f1 * z.x, @f1 * z.y) +
        @c2 * stream_func_gradient_X(@f2 * z.x, @f2 * z.y) +
        @c3 * stream_func_gradient_X(@f3 * z.x, @f3 * z.y),
        @c1 * stream_func_gradient_Y(@f1 * z.x, @f1 * z.y) +
        @c2 * stream_func_gradient_Y(@f2 * z.x, @f2 * z.y) +
        @c3 * stream_func_gradient_Y(@f3 * z.x, @f3 * z.y));
__bailout:
//=| factal bailout function
    // must always define bailedOut!!!
    bailedOut = (dot(z, z) > bailout*bailout);
__functions:
real stream_func_gradient_X(const real xold, const real yold)
{
    real a = 1. / sqrt(3.);
    real f = sin(2. * a * xold);
    real g = sin(a * xold + yold);
    real h = sin(a * xold - yold);
    real gy = cos(a * xold + yold);
    real hy = -cos(a * xold - yold);
    return f*gy*h + f*g*hy;
}
real stream_func_gradient_Y(const real xold, const real yold)
{
    real a = 1. / sqrt(3.);
    real f = sin(2. * a * xold);
    real g = sin(a * xold + yold);
    real h = sin(a * xold - yold);
    real fx = 2. * a * cos(2. * a * xold);
    real gx = a * cos(a * xold + yold);
    real hx = a * cos(a * xold - yold);
    return -(fx*g*h + f*gx*h + f*g*hx);
}