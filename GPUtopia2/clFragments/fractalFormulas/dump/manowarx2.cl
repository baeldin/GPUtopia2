__parameters:
float parameter exponentR = 2.f;
float parameter exponentI = 0.f;
float parameter exponent2R = 2.f;
float parameter exponent2I = 0.f;
int parameter updateZold = 0;
__init:
//=====| fractal formula init
        complex z = z0;
        complex zf_old = z;
        complex z1 = z;
        complex exponent = (complex)(@exponentR, @exponentI);
        complex exponent2 = (complex)(@exponent2R, @exponent2I);
__loop:
//=========| fractal formula loop
            zf_old = z;
            z = cpow(z, exponent) + z1 + z0;
            if (@updateZold > 0)
            {
                z1 = zf_old;
                zf_old = z;
            }
            z = cpow(z, exponent2) + z1 + z0;
            z1 = zf_old;
__bailout:
//=| factal bailout function
    // must always define bailedOut!!!
    bailedOut = (dot(z, z) > bailout);
__functions:
    //=| functions which are usable in the init, loop, and bailout parts