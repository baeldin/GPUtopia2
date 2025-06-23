__parameters:
__init:
//=====| fractal formula init
        complex z = z0;
    __loop:
//=========| fractal formula loop
             z = cexp(z) + z0;
__bailout:
//=| factal bailout function
    // must always define bailedOut!!!
    bailedOut = (dot(z, z) > bailout);
__functions:
//=| functions which are usable in the init, loop, and bailout parts
