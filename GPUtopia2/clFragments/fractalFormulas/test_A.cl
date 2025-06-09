__parameters:
complex parameter exponent = (2., 0.);
__init:
//=====| fractal formula init
        complex z = z0;
    __loop:
//=========| fractal formula loop
             z = cpow(conj(z), @exponent) - z + z0;
__bailout:
//=| factal bailout function
    // must always define bool bailedout!!!
    bool bailedout = (dot(z, z) > bailout);
__functions:
//=| functions which are usable in the init, loop, and bailout parts
float t(int i,
    float b)
{
    return 2.f*(float)i+b;
}