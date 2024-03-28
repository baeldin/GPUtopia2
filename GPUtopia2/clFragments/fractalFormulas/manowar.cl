__parameters:
__init:
//=====| fractal formula init
        float2 z = (float2)(0.f, 0.f);
        float2 zf_old = z;
        float2 z1 = z;
__loop:
//=========| fractal formula loop
            zf_old = z;
            z = (float2)(
                z.x * z.x - z.y * z.y + z1.x + z0.x,
                2.f * z.x * z.y + z1.y + z0.y);
            z1 = zf_old;
__bailout:
//=| factal bailout function
    // must always define bool bailedout!!!
    bool bailedout = (dot(z, z) > bailout);