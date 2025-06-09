__parameters:
    enum parameter Type1 = 0;
    Square, Trignarls, Pentarls, Round
    float parameter c1 = 0.1f;
    float parameter f1 = 1.f
    float parameter N1 = 3.f;
    complex parameter shift1 = (0., 0.);
    float parameter rot1 = 0.f;
    float parameter roundShift1 = 2.5f;
    enum parameter Type2 = 0;
    Square, Trignarls, Pentarls, Round
    float parameter c2 = .1f;
    float parameter f2 = 1.f;
    float parameter N2 = 3.f;
    complex parameter shift2 = (0., 0.);
    float parameter rot2 = 0.f;
    float parameter roundShift2 = 2.5f;
    enum parameter Type3 = 0;
    Square, Trignarls, Pentarls, Round
    float parameter c3 = 0.1f;
    float parameter f3 = 1.f;
    float parameter N3 = 3.f;
    complex parameter shift3 = (0., 0.);
    float parameter rot3 = 0.f;
    float parameter roundShift3 = 2.5f;
    float parameter scale = 1.f;
    enum parameter numericalMethod = 0;
    Midpoint, RK4
__init:
//=====| fractal formula init
        real2 z = z0;
        const real deg2rad = 0.017453292519943295769236907684886127;
        real rot1rad = deg2rad * @rot1;
        real rot2rad = deg2rad * @rot2;
        real rot3rad = deg2rad * @rot3;
        complex crot1 = (complex)(cos(rot1rad), sin(rot1rad));
        complex crot2 = (complex)(cos(rot2rad), sin(rot2rad));
        complex crot3 = (complex)(cos(rot3rad), sin(rot3rad));        
__loop:
//=========| fractal formula loop
        if (@numericalMethod == 0)
        {
            if (@Type1 == 0)
                z += @c1 * MP_square(@f1 * cmul(z, crot1) + @shift1, @scale * @c1);
            else if (@Type1 == 1)
                z += @scale * @c1 * MP_tri(@f1 * cmul(z, crot1) + @shift1, @scale * @c1);
            else if (@Type1 == 2)
                z += @scale * @c1 * MP_pent(@f1 * cmul(z, crot1) + @shift1, @scale * @c1);
            else if (@Type1 == 3)
                z += @scale * @c1 * MP_round(@f1 * cmul(z, crot1) + @shift1, @N1, @roundShift1, @scale * @c1);
            if (@Type2 == 0)
                z += @scale * @c2 * MP_square(@f2 * cmul(z, crot2) + @shift2, @scale * @c2);
            else if (@Type2 == 1)
                z += @scale * @c2 * MP_tri(@f2 * cmul(z, crot2) + @shift2, @scale * @c2);
            else if (@Type2 == 2)
                z += @scale * @c2 * MP_pent(@f2 * cmul(z, crot2) + @shift2, @scale * @c2);
            else if (@Type2 == 3)
                z += @scale * @c2 * MP_round(@f2 * cmul(z, crot2) + @shift2, @N2, @roundShift2, @scale * @c2);
            if (@Type3 == 0)
                z += @scale * @c3 * MP_square(@f3 * cmul(z, crot3) + @shift3, @scale * @c3);
            else if (@Type3 == 1)
                z += @scale * @c3 * MP_tri(@f3 * cmul(z, crot3) + @shift3, @scale * @c3);
            else if (@Type3 == 2)
                z += @scale * @c3 * MP_pent(@f3 * cmul(z, crot3) + @shift3, @scale * @c3);
            else if (@Type3 == 3)
                z += @scale * @c3 * MP_round(@f3 * cmul(z, crot3) + @shift3, @N3, @roundShift3, @scale * @c3);
        }
        else
        if (@numericalMethod == 1)
        {
            if (@Type1 == 0)
                z += @scale * @c1 * RK4_square(@f1 * cmul(z, crot1) + @shift1, @scale * @c1);
            else if (@Type1 == 1)
                z += @scale * @c1 * RK4_tri(@f1 * cmul(z, crot1) + @shift1, @scale * @c1);
            else if (@Type1 == 2)
                z += @scale * @c1 * RK4_pent(@f1 * cmul(z, crot1) + @shift1, @scale * @c1);
            else if (@Type1 == 3)
                z += @scale * @c1 * RK4_round(@f1 * cmul(z, crot1) + @shift1, @N1, @roundShift1, @scale * @c1);
            if (@Type2 == 0)
                z += @scale * @c2 * RK4_square(@f2 * cmul(z, crot2) + @shift2, @scale * @c2);
            else if (@Type2 == 1)
                z += @scale * @c2 * RK4_tri(@f2 * cmul(z, crot2) + @shift2, @scale * @c2);
            else if (@Type2 == 2)
                z += @scale * @c2 * RK4_pent(@f2 * cmul(z, crot2) + @shift2, @scale * @c2);
            else if (@Type2 == 3)
                z += @scale * @c2 * RK4_round(@f2 * cmul(z, crot2) + @shift2, @N2, @roundShift2, @scale * @c2);
            if (@Type3 == 0)
                z += @scale * @c3 * RK4_square(@f3 * cmul(z, crot3) + @shift3, @scale * @c3);
            else if (@Type3 == 1)
                z += @scale * @c3 * RK4_tri(@f3 * cmul(z, crot3) + @shift3, @scale * @c3);
            else if (@Type3 == 2)
                z += @scale * @c3 * RK4_pent(@f3 * cmul(z, crot3) + @shift3, @scale * @c3);
            else if (@Type3 == 3)
                z += @scale * @c3 * RK4_round(@f3 * cmul(z, crot3) + @shift3, @N3, @roundShift3, @scale * @c3);
        }
__bailout:
//=| factal bailout function
    // must always define bool bailedout!!!
    bool bailedout = (dot(z, z) > bailout*bailout);
__functions:
//=| functions which are usable in the init, loop, and bailout parts
__constant real i6f = 1. / 6.;
// TRIGNARL CONSTANTS

real2 trignarl_func(const complex z)
{
    const real isqrt3 = 0.577350269189625764509148780501957455;
    const real isqrt3x2 = 1.154700538379251529018297561003914911;    real f = sin(isqrt3x2 * z.x);
    real g = sin(isqrt3 * z.x + z.y);
    real h = sin(isqrt3 * z.x - z.y);
    real gy = cos(isqrt3 * z.x + z.y);
    real hy = -cos(isqrt3 * z.x - z.y);
    real fx = 2. * isqrt3 * cos(isqrt3x2 * z.x);
    real gx = isqrt3 * cos(isqrt3 * z.x + z.y);
    real hx = isqrt3 * cos(isqrt3 * z.x - z.y);
    return (real2)(f*gy*h + f*g*hy, -(fx*g*h + f*gx*h + f*g*hx));
}
complex RK4_tri(const complex v, const real h)
{
    const complex k1 = trignarl_func(v);
    const complex k2 = trignarl_func(v + (real)0.5 * h * k1);
    const complex k3 = trignarl_func(v + (real)0.5 * h * k2);
    const complex k4 = trignarl_func(v + h * k3);
    return i6f * h * (k1 + (real)2. * k2 + (real)2. * k3 + k4);
}
complex MP_tri(const complex v, const real h)
{
    const complex k1 = trignarl_func(v);
    return trignarl_func(v + (real)0.5 * h * k1);
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
// ROUND GNARLS
complex round_gnarls(const complex z, const real N, const real shift)
{
  const real pi = 3.14159265359;
  real rq = z.x*z.x + z.y*z.y;
  real r = sqrt(rq);
  real R_ = floor(r/pi);
  real Phi = atan2(z.y, z.x);
  real angleShift = pow(R_, shift);
  real arg2 = (N * R_ + (real)2.) * Phi + angleShift;
  real p2 = (R_ + (real)2.) * sin(r) * cos(arg2) / rq;
  real p1 = cos(r) * sin(arg2) / r;
  real fx = z.x * p1 - N * z.y * p2;
  real fy = z.y * p1 + N * z.x * p2;
  return (complex)(fy, -fx);
}
complex RK4_pent(const complex v, const real h)
{
    const real i6f = 1. / 6.;
    const complex k1 = stream_func_pentarls2_X(v);
    const complex k2 = stream_func_pentarls2_X(v + (real)0.5 * h * k1);
    const complex k3 = stream_func_pentarls2_X(v + (real)0.5 * h * k2);
    const complex k4 = stream_func_pentarls2_X(v + h * k3);
    return i6f * h * (k1 + (real)2. * k2 + (real)2. * k3 + k4);
}
complex MP_pent(const complex v, const real h)
{
    const complex k1 = stream_func_pentarls2_X(v);
    return stream_func_pentarls2_X(v + (real)0.5 * h * k1);
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
complex MP_square(const complex v, const real h)
{
    const complex k1 = f(v);
    return f(v + (real)0.5 * h * k1);
}
complex RK4_round(const complex v, const real N, const real shift, const real h)
{
    const real i6f = 1. / 6.;
    const complex k1 = round_gnarls(v, N, shift);
    const complex k2 = round_gnarls(v + (real)0.5 * h * k1, N, shift);
    const complex k3 = round_gnarls(v + (real)0.5 * h * k2, N, shift);
    const complex k4 = round_gnarls(v + h * k3, N, shift);
    return i6f * h * (k1 + (real)2. * k2 + (real)2. * k3 + k4);
}
complex MP_round(const complex v, const real N, const real shift, const real h)
{
    const complex k1 = round_gnarls(v, N, shift);
    return round_gnarls(v + (real)0.5 * h * k1, N, shift);
}