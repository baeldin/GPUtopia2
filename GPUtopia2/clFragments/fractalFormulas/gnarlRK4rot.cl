__parameters:
    float parameter c1 = 0.01f;
    float parameter f1 = 5.f
    float parameter c2 = 0.05f;
    float parameter f2 = 15.f;
    float parameter c3 = 0.005f;
    float parameter f3 = 45.f;
    float parameter offsetReal = 0.f;
    float parameter offsetImag = 0.001f;
    complex parameter rot = (1., 0.);
__init:
//=====| fractal formula init
        real2 z = z0;
__loop:
//=========| fractal formula loop
        z += RK4(cmul(@rot, z), @f1, @c1) + RK4(cmul(@rot, z), @f2, @c2) + RK4(cmul(@rot, z), @f3, @c3) + (real2)(@offsetReal, @offsetImag);
__bailout:
//=| factal bailout function
    // must always define bailedOut!!!
    bailedOut = (dot(z, z) > bailout*bailout);
__functions:
//=| functions which are usable in the init, loop, and bailout parts
    real i6f = 1.f / 6.f;
    real2 f(real2 v, float freq)
    {
        // f ........ frequency
        return (real2)(cos(freq * v.y), cos(freq * v.x));
    }
    real2 RK4(real2 v, real freq, real h)
    {
        const real2 k1 = f(v, freq);
        const real2 k2 = f(v + 0.5f * h * k1, freq);
        const real2 k3 = f(v + 0.5f * h * k2, freq);
        const real2 k4 = f(v + h * k3, freq);
        return i6f * h * (k1 + 2.f * k2 + 2.f * k3 + k4);
    }