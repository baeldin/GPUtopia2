__parameters:
__init:
//=====| fractal formula init
        real2 z = (real2)(0.f, 0.f);
__loop:
//=========| fractal formula loop
            z = cexp((real2)(
                z.x * z.x - z.y * z.y + z0.x,
                2.f * z.x * z.y + z0.y)) + z0;
__bailout:
//=| factal bailout function
    // must always define bailedOut!!!
    bailedOut = (dot(z, z) > bailout);
__functions:
//=| functions which are usable in the init, loop, and bailout parts