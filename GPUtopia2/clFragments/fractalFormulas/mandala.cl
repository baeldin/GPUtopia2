__parameters:
float parameter exp1R = 2.f;
float parameter exp1I = 0.f;
float parameter exp2R = 2.f;
float parameter exp2I = 0.f;
__init:
//=====| fractal formula init
        complex z = z0; // (float2)(0.f, 0.f);
        complex exp1 = (complex)(@exp1R, @exp1I);
        complex exp2 = (complex)(@exp2R, @exp2I);
        complex exp2inv = cdiv((float2)(1., 0.), exp2);
    __loop:
//=========| fractal formula loop
            z = cabs(cpow(z, exp1));
            z = cpow(z, exp2inv);
            z += z0;
__bailout:
//=| factal bailout function
    // must always define bailedOut!!!
    bailedOut = (dot(z, z) > bailout);
__functions:
//=| functions which are usable in the init, loop, and bailout parts