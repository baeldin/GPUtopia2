__parameters:
    float parameter c1 = 0.01f;
    float parameter f1 = 5.f
    float parameter c2 = 0.05f;
    float parameter f2 = 15.f;
    float parameter c3 = 0.005f;
    float parameter f3 = 45.f;
    float parameter offsetReal = 0.f;
    float parameter offsetImag = 0.001f;
__init:
//=====| fractal formula init
        float2 z = z0;
__loop:
//=========| fractal formula loop
            z += (float2)(
                @c1 * cos(@f1 * z.y) + @c2 * cos (@f2 * z.y) + @c3 * cos(@f3 * z.y) + @offsetReal,
                @c1 * cos(@f1 * z.x) + @c2 * cos (@f2 * z.x) + @c3 * cos(@f3 * z.x) + @offsetImag);
__bailout:
//=| factal bailout function
    // must always define bool bailedout!!!
    bool bailedout = (dot(z, z) > bailout*bailout);
__functions:
//=| functions which are usable in the init, loop, and bailout parts