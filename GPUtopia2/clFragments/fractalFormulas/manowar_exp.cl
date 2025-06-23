__parameters:
float parameter exponentR = 2.f;
float parameter exponentI = 0.f;
__init:
//=====| fractal formula init
        complex z = z0;
        complex z_old = {0.f, 0.f};
        complex z1 = {0.f, 0.f};
        complex z2 = {0.f, 0.f};
        complex exponent = (complex)(@exponentR, @exponentI);
__loop:
//=========| fractal formula loop
            z_old = z;
            z = cmul(ccos(z), z) + csin(z) + z1 + z0;
            z1 = z_old;
__bailout:
//=| factal bailout function
    // must always define bailedOut!!!
    bailedOut = (dot(z, z) > bailout);
__functions:
    //=| functions which are usable in the init, loop, and bailout parts