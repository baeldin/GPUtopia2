__parameters:
    float parameter c1 = 0.01f;
    float parameter f1 = 5.f
    float parameter c2 = 0.05f;
    float parameter f2 = 15.f;
__init:
//=====| fractal formula init
        real2 z = z0;
__loop:
//=========| fractal formula loop
            z += (real2)(
                @c1 * cos(@f1 * z.y),
                @c1 * cos(@f1 * z.x));
            z += (real2)(
                @c2 * cos (@f2 * z.y),
                @c2 * cos (@f2 * z.x));
__bailout:
//=| factal bailout function
    // must always define bool bailedout!!!
    bool bailedout = (dot(z, z) > bailout*bailout);
__functions:
// empty