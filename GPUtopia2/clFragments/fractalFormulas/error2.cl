__parameters:
    float parameter c1 = 0.1f;
__init:
//=====| fractal formula init
        real2 z = z0;
__loop:
//=========| fractal formula loop
        z += @c1 * midpoint_mb(z, z0, @c1);
__blout:
//=| factal bailout function
    // must always define bailedOut!!!
    bailedOut = (dot(z, z) > bailout*bailout);
__functions:
//=| functions which are usable in the init, loop, and bailout parts
// MB step
complex f(const complex z, const complex z0)
{
    return cmul(z, z) - z + z0;
}
complex midpoint_mb(const complex z, const complex z0, const real h)
{
    const complex k1 = f(z, z0);
    const complex k2 = f(z + (real)0.5 * h * k1, z0);
    return h * k2;
}