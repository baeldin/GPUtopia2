__parameters:
float parameter exponentR = 2.f;
float parameter exponentI = 0.f;
float parameter exponentR2 = 2.f;
float parameter exponentI2 = 0.f;
__init:
//=====| fractal formula init
        complex z = z0;
        complex exponent = (complex)(@exponentR, @exponentI);
        complex exponent2 = (complex)(@exponentR2, @exponentI2);
    __loop:
//=========| fractal formula loop
            //z = (float2)(
            //    z.x * z.x - z.y * z.y + z0.x,
            //    2.f * z.x * z.y + z0.y);
            z = cpow(z, exponent) + z0;
            z = cpow(z, exponent2) + z0;
__bailout:
//=| factal bailout function
    // must always define bailedOut!!!
    bailedOut = (dot(z, z) > bailout);
__functions:
//=| functions which are usable in the init, loop, and bailout parts