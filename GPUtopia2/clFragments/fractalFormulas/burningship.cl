__parameters:
float parameter expR = 2.;
float parameter expI = 0.;
__init:
//=====| fractal formula init
        complex z = z0;
        complex exp1 = (complex)(@expR, @expI);   
__loop:
//=========| fractal formula loop
            z = fabs(z);
            z = cpow(z, exp1) + z0;
__bailout:
//=| factal bailout function
    // must always define bool bailedout!!!
    bool bailedout = (dot(z, z) > bailout);
__functions:
//=| functions which are usable in the init, loop, and bailout parts