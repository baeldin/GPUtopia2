__parameters:
float parameter exp1R = 2.f;
float parameter exp1I = 0.f;
float parameter exp2R = 2.f;
float parameter exp2I = 0.f;
__init:
//=====| fractal formula init
        float2 z = (float2)(0.f, 0.f);
        complex exp1 = (complex)(@exp1R, @exp1I);
        complex exp2 = (complex)(@exp2R, @exp2I);
__loop:
//=========| fractal formula loop
            z = fabs(cpow(z, exp1));
            complex exp2inv = cdiv((float2)(1., 0.), exp2);
            z = cpow(z, exp2inv);
            z += z0;
__bailout:
//=| factal bailout function
    // must always define bool bailedout!!!
    bool bailedout = (dot(z, z) > bailout);
__functions:
//=| functions which are usable in the init, loop, and bailout parts