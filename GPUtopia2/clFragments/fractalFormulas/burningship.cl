__parameters:
__init:
//=====| fractal formula init
        float2 z = (float2)(0.f, 0.f);
__loop:
//=========| fractal formula loop
            z = fabs(z);
            z = (float2)(
                z.x * z.x - z.y * z.y + z0.x,
                2.f * z.x * z.y + z0.y);
__bailout:
//=| factal bailout function
    // must always define bool bailedout!!!
    bool bailedout = (dot(z, z) > bailout);