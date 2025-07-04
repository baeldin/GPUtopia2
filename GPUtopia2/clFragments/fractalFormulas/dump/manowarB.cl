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
            // z1 = cmul(cmul(cmul(z, z), z), z) + z_old;
            // z2 = cmul(z, z) - cmul(z, z_old) + cmul(z_old, z_old);
            z = cmul(z, z) + z1 + z2 + z0;
            z1 = csin(z);
            z2 = ccos(z1);
__bailout:
//=| factal bailout function
    // must always define bailedOut!!!
    bailedOut = (dot(z, z) > bailout);
__functions:
    //=| functions which are usable in the init, loop, and bailout parts