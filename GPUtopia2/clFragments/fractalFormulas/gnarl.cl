__parameters:
    real parameter c1 = 0.01f;
    real parameter f1 = 5.f
    real parameter c2 = 0.05f;
    real parameter f2 = 15.f;
    real parameter c3 = 0.005f;
    real parameter f3 = 45.f;
    real parameter offsetReal = 0.f;
    real parameter offsetImag = 0.001f;
__init:
//=====| fractal formula init
        real2 z = z0;
__loop:
//=========| fractal formula loop
            z += (real2)(
                @c1 * cos(@f1 * z.y) + @c2 * cos (@f2 * z.y) + @c3 * cos(@f3 * z.y) + @offsetReal,
                @c1 * cos(@f1 * z.x) + @c2 * cos (@f2 * z.x) + @c3 * cos(@f3 * z.x) + @offsetImag);
__bailout:
//=| factal bailout function
    // must always define bool bailedout!!!
    bool bailedout = (dot(z, z) > bailout*bailout);
__functions:
//=| functions which are usable in the init, loop, and bailout parts