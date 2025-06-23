__parameters:
    float parameter c1 = 0.01f;
    float parameter f1 = 5.f
    float parameter c2 = 0.01f;
    float parameter f2 = 10.f
    float parameter pentarlOffset = 0.2f;
    float parameter offsetReal = 0.f;
    float parameter offsetImag = 0.001f;
__init:
//=====| fractal formula init
        complex z = z0;
        const real os = @pentarlOffset;
        const real pi = 3.1415926f;
        const real deg2rad = pi / 180.f;
        const real a1 = cos(deg2rad * 72.f);
        const real a2 = cos(deg2rad * 144.f);
        const real a3 = a1;
        const real a4 = a2;
        const real b1 = sin(deg2rad * 72.f);
        const real b2 = sin(deg2rad * 144.f);
        const real b3 = -b1;
        const real b4 = -b2; 
__loop:
//=========| fractal formula loop
            z += (real2)(
                @c1 * stream_func_pentarls2_X(@f1 * z.x, @f1 * z.y, a1, a2, a3, a4, b1, b2, b3, b4, os) + 
                @c2 * stream_func_pentarls2_X(@f2 * z.x, @f2 * z.y, a1, a2, a3, a4, b1, b2, b3, b4, os) +
                @offsetReal,
                @c1 * stream_func_pentarls2_Y(@f1 * z.x, @f1 * z.y, a1, a2, a3, a4, b1, b2, b3, b4, os) +
                @c2 * stream_func_pentarls2_Y(@f2 * z.x, @f2 * z.y, a1, a2, a3, a4, b1, b2, b3, b4, os) +
                @offsetImag);
__bailout:
//=| factal bailout function
    // must always define bailedOut!!!
    bailedOut = (dot(z, z) > bailout*bailout);
__functions:
//=| functions which are usable in the init, loop, and bailout parts

  real stream_func_pentarls2_X(const real x, const real y,        \
    const real a1, const real a2, const real a3, const real a4,            \
    const real b1, const real b2, const real b3, const real b4,            \
    const real os)
{
    real gy = -b1*sin(a1*x+b1*y+os);
    real hy = -b2*sin(a2*x+b2*y+os);
    real iy = -b3*sin(a3*x+b3*y+os);
    real jy = -b4*sin(a4*x+b4*y+os);
    return gy+hy+iy+jy;
}
  real stream_func_pentarls2_Y(const real x, const real y,        \
    const real a1, const real a2, const real a3, const real a4,            \
    const real b1, const real b2, const real b3, const real b4,            \
    const real os)
  {
    real fx = -sin(x + os);
    real gx = -a1*sin(a1*x+b1*y+os);
    real hx = -a2*sin(a2*x+b2*y+os);
    real ix = -a3*sin(a3*x+b3*y+os);
    real jx = -a4*sin(a4*x+b4*y+os);
    return -(fx+gx+hx+ix+jx);
  }

