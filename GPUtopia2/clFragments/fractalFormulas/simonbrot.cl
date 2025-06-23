__parameters:
__init:
//=====| fractal formula init
        float2 z = (float2)(0.f, 0.f);
__loop:
//=========| fractal formula loop
            z = cmul(fabs(z), cmul(fabs(z), cmul(z, z))) + z0;
__bailout:
//=| factal bailout function
    // must always define bailedOut!!!
    bailedOut = (dot(z, z) > bailout);
__functions:
//=| functions which are usable in the init, loop, and bailout parts