__parameters:
float parameter expR = 2.;
float parameter expI = 0.;
__init:
//=====| fractal formula init
        complex z = z0;
        complex exp1 = (complex)(@expR, @expI);   
__loop:
//=========| fractal formula loop
            // z = rabs(rabs(z^2) + abs(z.y)(z.y+abs(z.y))) + c;
            z = rabs(rabs(cmul(z, z) + fabs(z.y) * (z.y + fabs(z.y)))) + z0;
__bailout:
//=| factal bailout function
    // must always define bailedOut!!!
    bailedOut = (dot(z, z) > bailout);
__functions:
//=| functions which are usable in the init, loop, and bailout parts