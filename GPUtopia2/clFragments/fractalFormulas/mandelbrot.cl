__parameters:
complex parameter exponent = (2., 0.);
enum parameter test = 0;
zero, one, two, three
__init:
//=====| fractal formula init
        complex z = z0;
    __loop:
//=========| fractal formula loop
             //z = (complex)(
             //    z.x * z.x - z.y * z.y + z0.x,
             //    2.f * z.x * z.y + z0.y);
             z = cpow(z, @exponent) + z0;
__bailout:
//=| factal bailout function
    // must always define bool bailedout!!!
    bool bailedout = (dot(z, z) > bailout);
__functions:
//=| functions which are usable in the init, loop, and bailout parts