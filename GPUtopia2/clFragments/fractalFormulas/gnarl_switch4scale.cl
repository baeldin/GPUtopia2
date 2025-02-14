__parameters:
    enum parameter Type1 = 0;
    Square, Trignarls, Pentarls
    float parameter c1 = 0.1f;
    float parameter f1 = 1.f
    enum parameter Type2 = 0;
    Square, Trignarls, Pentarls
    float parameter c2 = .1f;
    float parameter f2 = 1.f;
    enum parameter Type3 = 0;
    Square, Trignarls, Pentarls
    float parameter c3 = 0.1f;
    float parameter f3 = 1.f;
    float parameter scale = 1.f;
__init:
//=====| fractal formula init
        real2 z = z0;
__loop:
//=========| fractal formula loop
        if (@Type1 == 0)
            z += @c1 * RK4_square(@f1 * z, @scale * @c1);
        else if (@Type1 == 1)
            z += @scale * @c1 * RK4_tri(@f1 * z, @scale * @c1);
        else if (@Type1 == 2)
            z += @scale * @c1 * RK4_pent(@f1 * z, @scale * @c1);
        if (@Type2 == 0)
            z += @scale * @c2 * RK4_square(@f2 * z, @scale * @c2);
        else if (@Type2 == 1)
            z += @scale * @c2 * RK4_tri(@f2 * z, @scale * @c2);
        else if (@Type2 == 2)
            z += @scale * @c2 * RK4_pent(@f2 * z, @scale * @c2);
        if (@Type3 == 0)
            z += @scale * @c3 * RK4_square(@f3 * z, @scale * @c3);
        else if (@Type3 == 1)
            z += @scale * @c3 * RK4_tri(@f3 * z, @scale * @c3);
        else if (@Type3 == 2)
            z += @scale * @c3 * RK4_pent(@f3 * z, @scale * @c3);
__bailout:
//=| factal bailout function
    // must always define bool bailedout!!!
    bool bailedout = (dot(z, z) > bailout*bailout);
__functions:
//=| functions which are usable in the init, loop, and bailout parts
__constant real i6f = 1. / 6.;
// TRIGNARL CONSTANTS
__constant real isqrt3 = 0.57735026918962576450914878050195745564760175127012687601860232648397767230;
__constant real isqrt3x2 = 1.15470053837925152901829756100391491129520350254025375203720465296795534460;
real2 trignarl_func(const complex z)
{
    real f = sin(isqrt3x2 * z.x);
    real g = sin(isqrt3 * z.x + z.y);
    real h = sin(isqrt3 * z.x - z.y);
    real gy = cos(isqrt3 * z.x + z.y);
    real hy = -cos(isqrt3 * z.x - z.y);
    real fx = 2. * isqrt3 * cos(isqrt3x2 * z.x);
    real gx = isqrt3 * cos(isqrt3 * z.x + z.y);
    real hx = isqrt3 * cos(isqrt3 * z.x - z.y);
    return (real2)(f*gy*h + f*g*hy, -(fx*g*h + f*gx*h + f*g*hx));
}
complex RK4_tri(complex v, real h)
{
    const complex k1 = trignarl_func(v);
    const complex k2 = trignarl_func(v + (real)0.5 * h * k1);
    const complex k3 = trignarl_func(v + (real)0.5 * h * k2);
    const complex k4 = trignarl_func(v + h * k3);
    return i6f * h * (k1 + (real)2. * k2 + (real)2. * k3 + k4);
}
// PENTARL CONSTANTS
// Pa3 =  Pa1, Pa4 =  Pa2 because cos has even parity
// Pb3 = -Pb1, Pb4 = -Pb2 because sin has odd parity
complex stream_func_pentarls2_X(const complex z)
{
    const real Pa1 = 0.30901699437494742410229341718281905886015458990288143106772431135263023140;
    const real Pa2 = -0.8090169943749474241022934171828190588601545899028814310677243113526302314;
    const real Pb1 = 0.95105651629515357211643933337938214340569863412575022244730564443015317008;
    const real Pb2 = 0.58778525229247312916870595463907276859765243764314599107227248075727847416;
    real gy = -Pb1*sin(Pa1*z.x+Pb1*z.y);
    real hy = -Pb2*sin(Pa2*z.x+Pb2*z.y);
    real iy =  Pb1*sin(Pa1*z.x-Pb1*z.y);
    real jy =  Pb2*sin(Pa2*z.x-Pb2*z.y);
    real fx = -sin(z.x);
    real gx = -Pa1*sin(Pa1*z.x+Pb1*z.y);
    real hx = -Pa2*sin(Pa2*z.x+Pb2*z.y);
    real ix = -Pa1*sin(Pa1*z.x-Pb1*z.y);
    real jx = -Pa2*sin(Pa2*z.x-Pb2*z.y);
    return (complex)(gy+hy+iy+jy, -(fx+gx+hx+ix+jx));
}
complex RK4_pent(complex v, real h)
{
    const real i6f = 1. / 6.;
    const complex k1 = stream_func_pentarls2_X(v);
    const complex k2 = stream_func_pentarls2_X(v + (real)0.5 * h * k1);
    const complex k3 = stream_func_pentarls2_X(v + (real)0.5 * h * k2);
    const complex k4 = stream_func_pentarls2_X(v + h * k3);
    return i6f * h * (k1 + (real)2. * k2 + (real)2. * k3 + k4);
}
// square gnarl
complex f(const complex z)
{
    return (complex)(cos(z.y), cos(z.x));
}
complex RK4_square(complex v, real h)
{
    const complex k1 = f(v);
    const complex k2 = f(v + (real)0.5 * h * k1);
    const complex k3 = f(v + (real)0.5 * h * k2);
    const complex k4 = f(v + h * k3);
    return i6f * h * (k1 + (real)2. * k2 + (real)2. * k3 + k4);
}