__parameters:
    float parameter c1 = 0.01f;
    float parameter f1 = 5.f
    float parameter c2 = 0.05f;
    float parameter c3 = 0.005f;
__init:
//=====| fractal formula init
        float2 z = z0;
__loop:
//=========| fractal formula loop
            z += (float2)(
                @c1 * cos(@f1 * z.y) + @c2 * cos (15.f * z.y) + @c3 * cos(45.f * z.y),
                @c1 * cos(@f1 * z.x) + @c2 * cos (15.f * z.x) + @c3 * cos(45.f * z.x) + 0.001f);
__bailout:
//=| factal bailout function
    // must always define bool bailedout!!!
    bool bailedout = (dot(z, z) > bailout*bailout);