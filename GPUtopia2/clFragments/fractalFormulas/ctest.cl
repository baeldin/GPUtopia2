__parameters:
__init:
//=====| fractal formula init
complex z;
z.x = z0.x;
z.y = z0.y;
__loop:
//=========| fractal formula loop
        z = cmul(csin(z), z);
        z.x += z0.x;
        z.y += z0.y;
__bailout:
//=| factal bailout function
    // must always define bool bailedout!!!
    bool bailedout = (dot(z, z) > bailout);
__functions:
//=| functions which are usable in the init, loop, and bailout parts