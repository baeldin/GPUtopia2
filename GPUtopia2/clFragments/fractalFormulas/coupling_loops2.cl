__parameters:
float parameter exponent1R = 2.f;
float parameter exponent1I = 0.f;
float parameter exponent2R = 2.f;
float parameter exponent2I = 0.f;
float parameter coupling_shift = 50.f;
float parameter cR = 0.f;
float parameter cI = 0.1f;
float parameter seedR = -0.750456803963f;
float parameter seedI = 0.0328866621095f;
__init:
//=====| fractal formula init
        complex z = z0;
        const complex exponent1 = (complex)(@exponent1R, @exponent1I);
        const complex exponent2 = (complex)(@exponent2R, @exponent2I);
        const complex seed = (complex)(@seedR, @seedI);
        const complex c = (complex)(@cR, @cI);
        int k = 0;
        float kmax = 0.f;
    __loop:
//=========| fractal formula loop
            if (k < kmax)
            {
                z = cpow(z, exponent1) + z0;
                k = k + 1;
            } else {
                z = cpow(z, exponent1) + cmul(c, cpow(z, exponent2)) + z0;
                k = 0;
                kmax += @coupling_shift;
            }
__bailout:
//=| factal bailout function
    // must always define bailedOut!!!
    bailedOut = (dot(z, z) > bailout);
__functions:
//=| functions which are usable in the init, loop, and bailout parts