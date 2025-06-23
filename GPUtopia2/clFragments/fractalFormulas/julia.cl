__parameters:
float parameter exponentR = 2.f;
float parameter exponentI = 0.f;
float parameter seedR = -0.6.f;
float parameter seedI = 0.4.f;
__init:
//=====| fractal formula init
        complex z = z0;
        complex exponent = (complex)(@exponentR, @exponentI);
        complex seed = (complex)(@seedR, @seedI);
    __loop:
//=========| fractal formula loop
            //z = (float2)(
            //    z.x * z.x - z.y * z.y + z0.x,
            //    2.f * z.x * z.y + z0.y);
            z = cpow(z, exponent) + seed;
__bailout:
//=| factal bailout function
    // must always define bailedOut!!!
    bailedOut = (dot(z, z) > bailout);
__functions:
//=| functions which are usable in the init, loop, and bailout parts