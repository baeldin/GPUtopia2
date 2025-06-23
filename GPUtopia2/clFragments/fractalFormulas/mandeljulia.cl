__parameters:
float parameter juliaX = -0.6f;
float parameter juliaY = 0.4f;
__init:
//=====| fractal formula init
        float2 z = (float2)(0.f, 0.f);
__loop:
//=========| fractal formula loop
            z = (float2)(
                z.x * z.x - z.y * z.y + z0.x,
                2.f * z.x * z.y + z0.y);
            z = (float2)(
                z.x * z.x - z.y * z.y + @juliaX,
                2.f * z.x * z.y + @juliaY);
__bailout:
//=| factal bailout function
    // must always define bailedOut!!!
    bailedOut = (dot(z, z) > bailout);
__functions:
//=| functions which are usable in the init, loop, and bailout parts