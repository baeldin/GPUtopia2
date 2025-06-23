__parameters:
float parameter exponentR = 2.f;
float parameter exponentI = 0.f;
__init:
//=====| fractal formula init
complex z = z0;
complex exponent = (complex)(@exponentR, @exponentI);
__loop:
//=========| fractal formula loop
            //z = (float2)(
            //    z.x * z.x - z.y * z.y + z0.x,
            //    2.f * z.x * z.y + z0.y);
z = (complex)(z.x * z.x - z.y * z.y + z0.x, 2.f * absf(z.x) * z.y + z0.y);
__bailout:
//=| factal bailout function
    // must always define bailedOut!!!
bailedOut = (dot(z, z) > bailout);
__functions:
//=| functions which are usable in the init, loop, and bailout parts