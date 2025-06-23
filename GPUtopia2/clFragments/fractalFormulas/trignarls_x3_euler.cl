__parameters:
    float parameter c1 = 0.01f;
    float parameter f1 = 5.f
    float parameter c2 = 0.01f;
    float parameter f2 = 10.f
    float parameter c3 = 0.01f;
    float parameter f3 = 15.f
    float parameter pentarlOffset = 0.2f;
    float parameter offsetReal = 0.f;
    float parameter offsetImag = 0.001f;
    int parameter mode = 0;
__init:
//=====| fractal formula init
        complex z = z0;
        const real os = @pentarlOffset;
        const real pi = 3.1415926535897932384626433f;
        const real deg2rad = pi / 180.;
        const real4 a = { cos(deg2rad * 72.f), cos(deg2rad * 144.f),  cos(deg2rad * 72.f),  cos(deg2rad * 144.f) };
        const real4 b = { sin(deg2rad * 72.f), sin(deg2rad * 144.f), -sin(deg2rad * 72.f), -sin(deg2rad * 144.f) };
__loop:
//=========| fractal formula loop
        if (@mode == 0)
        {
            z += Euler(@f1 * z, @c1, a, b) + Euler(@f2 * z, @c2, a, b) + Euler(@f3 * z, @c3, a, b) + (real2)(@offsetReal, @offsetImag);
        }
        else if (@mode == 1)
        {
            z += Euler(@f1 * z, @c1, a, b); 
            z += Euler(@f2 * z, @c2, a, b); 
            z += Euler(@f3 * z, @c3, a, b); 
            z += (real2)(@offsetReal, @offsetImag);
        }
__bailout:
//=| factal bailout function
    // must always define bailedOut!!!
    bailedOut = (dot(z, z) > bailout*bailout);
__functions:
//=| functions which are usable in the init, loop, and bailout parts
//__constant real isqrt3 = 1. / sqrt(3.);
real2 trignarl_func(const real2 v)
{
    const real isqrt3 = 1. / sqrt(3.);
    real f = sin(2. * isqrt3 * v.x);
    real g = sin(isqrt3 * v.x + v.y);
    real h = sin(isqrt3 * v.x - v.y);
    real gy = cos(isqrt3 * v.x + v.y);
    real hy = -cos(isqrt3 * v.x - v.y);
    real fx = 2. * isqrt3 * cos(2. * isqrt3 * v.x);
    real gx = isqrt3 * cos(isqrt3 * v.x + v.y);
    real hx = isqrt3 * cos(isqrt3 * v.x - v.y);
    return (real2)(f*gy*h + f*g*hy, -(fx*g*h + f*gx*h + f*g*hx));

}
real2 pentarl_func(const real2 v, const real4 a, const real4 b) //const real os)
{
    const real f = cos(v.x);
    const real g = cos(a[0] * v.x + b[0] * v.y);
    const real h = cos(a[1] * v.x + b[1] * v.y);
    const real i = cos(a[2] * v.x + b[2] * v.y);
    const real j = cos(a[3] * v.x + b[3] * v.y);
    // const real fy = 0.;
    const real gy = -b[0] * sin(a[0] * v.x + b[0] * v.y);
    const real hy = -b[1] * sin(a[1] * v.x + b[1] * v.y);
    const real iy = -b[2] * sin(a[2] * v.x + b[2] * v.y);
    const real jy = -b[3] * sin(a[3] * v.x + b[3] * v.y);
    const real fx = -sin(v.x);
    const real gx = -a[0] * sin(a[0] * v.x + b[0] * v.y);
    const real hx = -a[1] * sin(a[1] * v.x + b[1] * v.y);
    const real ix = -a[2] * sin(a[2] * v.x + b[2] * v.y);
    const real jx = -a[3] * sin(a[3] * v.x + b[3] * v.y);
    return (real2)(f * gy * h * i * j + 
                   f * g * hy * i * j + 
                   f * g * h * iy * j + 
                   f * g * h * i * jy,
                   fx * g * h * i * j + 
                   f * gx * h * i * j + 
                   f * g * hx * i * j + 
                   f * g * h * ix * j + 
                   f * g * h * i * jx);
}
const real i6f = 1.f / 6.f;
real2 RK4(const real2 v, const real h, const real4 a, const real4 b)
{
    const real2 k1 = trignarl_func(v);
    const real2 k2 = trignarl_func(v + (0.5f * h) * k1);
    const real2 k3 = trignarl_func(v + (0.5f * h) * k2);
    const real2 k4 = trignarl_func(v + h * k3);
    return i6f * h * (k1 + 2.f * k2 + 2.f * k3 + k4);
}
real2 Euler(const real2 v, const real h, const real4 a, const real4 b)
{
    const real2 k1 = trignarl_func(v);
    return h * k1;
}