__parameters:
__init:
//=====| fractal formula init
        float2 z = (float2)(0.f, 0.f);
        complex zold = (complex)(0.f, 0.f);
__loop:
//=========| fractal formula loop
            z = cdiv((cmul(z, z) + z0 - (float2)(1.f, 0.f)), (2.f * z + z0 - (float2)(2.f, 0.f)));
            z = cmul(z, z);
            zold = z;
__bailout:
//=| factal bailout function
    // must always define bool bailedout!!!
    bool bailedout = (dot(z, z) > bailout || dot(z, zold) < 0.00005);
__functions:
//=| functions which are usable in the init, loop, and bailout parts