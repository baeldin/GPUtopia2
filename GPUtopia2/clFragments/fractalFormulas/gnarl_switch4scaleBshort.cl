__parameters:
    float parameter c1 = 0.1f;
    float parameter f1 = 1.f
    float parameter c2 = .1f;
    float parameter f2 = 1.f;
    float parameter c3 = 0.1f;
    float parameter f3 = 1.f;
    float parameter scale = 1.f;
__init:
//=====| fractal formula init
        real2 z = z0;
__loop:
//=========| fractal formula loop
                z += @c1 * MP_square(@f1 * z, @scale * @c1);
                z += @scale * @c2 * MP_square(@f2 * z, @scale * @c2);
                z += @scale * @c3 * MP_square(@f3 * z, @scale * @c3);
__bailout:
//=| factal bailout function
    // must always define bool bailedout!!!
    bool bailedout = (dot(z, z) > bailout*bailout);
__functions:
//=| functions which are usable in the init, loop, and bailout parts
__constant real i6f = 1. / 6.;
// TRIGNARL CONSTANTS
// square gnarl
complex f(const complex z)
{
    return (complex)(cos(z.y), cos(z.x));
}
complex MP_square(const complex v, const real h)
{
    const complex k1 = f(v);
    return f(v + (real)0.5 * h * k1);
}