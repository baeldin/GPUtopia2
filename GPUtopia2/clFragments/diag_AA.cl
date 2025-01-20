__parameters:
__init:
//=====| fractal formula init
complex z = z0;
__loop:
//=========| fractal formula loop
__bailout:
//=| factal bailout function
    // must always define bool bailedout!!!
bool bailedout = true;
__functions:
//=| functions which are usable in the init, loop, and bailout parts