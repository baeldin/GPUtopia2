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
            z1 = ctan(cmul(z, z)) + z_old + 1.f;
            z2 = cmul(z, z) - z_old - 1.f;
            z = cdiv(z1, z2) + z0;
            z_old = z;
__bailout:
//=| factal bailout function
    // must always define bool bailedout!!!
    bool bailedout = (dot(z, z) > bailout);
__functions:
    //=| functions which are usable in the init, loop, and bailout parts