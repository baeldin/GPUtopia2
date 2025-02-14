__parameters:
complex parameter exponent = (2., 0.);
enum parameter test = 0;
Two, Three, ExponentArgument
__init:
//=====| fractal formula init
        complex z = z0;
    __loop:
//=========| fractal formula loop
             if (@test == 0)
                 z = cmul(z, z) + z0;
             else if (@test == 1)
                 z = cmul(cmul(z, z), z) + z0;
             else if (@test == 2)
                 z = cpow(z, @exponent) + z0;
__bailout:
//=| factal bailout function
    // must always define bool bailedout!!!
    bool bailedout = (dot(z, z) > bailout);
__functions:
//=| functions which are usable in the init, loop, and bailout parts