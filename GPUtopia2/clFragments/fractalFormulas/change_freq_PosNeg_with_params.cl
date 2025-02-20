__parameters:
    complex parameter exponent1 = (3, 0);
    complex parameter exponent2 = (0, 0);
    complex parameter exponent3 = (1, 0);
    complex parameter sc = (1, 0);
    real parameter di = 1.;
__init:
//=====| fractal formula init
        real2 z = z0;
        real k = 0;
        real r = 0.;
__loop:
//=========| fractal formula loop
        if (k < r)
        {
            z = cpow(z, @exponent1) + z0;
            k += (real)1.;
        }
        else
        {
            z = cpow(z, @exponent2) + cmul(@sc, cpow(z, @exponent3)) + z0;
            r += @di;
            k = 0;
        }
__bailout:
//=| factal bailout function
    // must always define bool bailedout!!!
    bool bailedout = (dot(z, z) > bailout*bailout);
__functions:
//=| functions which are usable in the init, loop, and bailout parts