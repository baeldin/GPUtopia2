__parameters:
float parameter exponentR = 2.f;
float parameter exponentI = 0.f;
complex parameter A = {1.f, 0.f};
complex parameter B = {0.2f, 0.1f};
__init:
//=====| fractal formula init
        complex z = z0;
        complex z_old = {0.f, 0.f};
        complex z1 = {0.f, 0.f};
        complex z2 = {0.f, 0.f};
        complex exponent = (complex)(@exponentR, @exponentI);
__loop:
//=========| fractal formula loop
            z = cpow(z, exponent) + @A * z1 + @B * z2 + z0;
            z2 = z1;
            z1 = z;
__bailout:
//=| factal bailout function
    // must always define bool bailedout!!!
    bool bailedout = (dot(z, z) > bailout);
__functions:
    //=| functions which are usable in the init, loop, and bailout parts