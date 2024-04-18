__parameters:
    float parameter c1 = 0.01f;
    float parameter f1 = 5.f
    float parameter c2 = 0.01f;
    float parameter f2 = 10.f
    float parameter pentarlOffset = 0.2f;
    float parameter offsetReal = 0.f;
    float parameter offsetImag = 0.001f;
__init:
//=====| fractal formula init
        float2 z = z0;
        float os = @pentarlOffset;
        const float pi = 3.1415926f;
        const float deg2rad = pi / 180.f;
        float a1 = cos(deg2rad * 72.f);
        float a2 = cos(deg2rad * 144.f);
        float a3 = a1;
        float a4 = a2;
        float b1 = sin(deg2rad * 72.f);
        float b2 = sin(deg2rad * 144.f);
        float b3 = -b1;
        float b4 = -b2; 
__loop:
//=========| fractal formula loop
            z += (float2)(
                @c1 * stream_func_pentarls_X(@f1 * z.x, @f1 * z.y, a1, a2, a3, a4, b1, b2, b3, b4, os) + 
                @c2 * stream_func_pentarls_X(@f2 * z.x, @f2 * z.y, a1, a2, a3, a4, b1, b2, b3, b4, os) +
                @offsetReal,
                @c1 * stream_func_pentarls_Y(@f1 * z.x, @f1 * z.y, a1, a2, a3, a4, b1, b2, b3, b4, os) +
                @c2 * stream_func_pentarls_Y(@f2 * z.x, @f2 * z.y, a1, a2, a3, a4, b1, b2, b3, b4, os) +
                @offsetImag);
__bailout:
//=| factal bailout function
    // must always define bool bailedout!!!
    bool bailedout = (dot(z, z) > bailout*bailout);
__functions:
//=| functions which are usable in the init, loop, and bailout parts

    float stream_func_pentarls_X(const float x, const float y,
        const float a1, const float a2, const float a3, const float a4,
        const float b1, const float b2, const float b3, const float b4,
        const float os)
    {
        const float f = cos(x);
        const float g = cos(a1 * x + b1 * y + os);
        const float h = cos(a2 * x + b2 * y + os);
        const float i = cos(a3 * x + b3 * y + os);
        const float j = cos(a4 * x + b4 * y + os);
        const float fx = -sin(x + os);
        const float gy = -b1 * sin(a1 * x + b1 * y + os);
        const float hy = -b2 * sin(a2 * x + b2 * y + os);
        const float iy = -b3 * sin(a3 * x + b3 * y + os);
        const float jy = -b4 * sin(a4 * x + b4 * y + os);
        return f * gy * h * i * j + f * g * hy * i * j + f * g * h * iy * j + f * g * h * i * jy;
    }
    float stream_func_pentarls_Y(const float x, const float y,
        const float a1, const float a2, const float a3, const float a4,
        const float b1, const float b2, const float b3, const float b4,
        const float os)
    {
        const float f = cos(x);
        const float g = cos(a1 * x + b1 * y + os);
        const float h = cos(a2 * x + b2 * y + os);
        const float i = cos(a3 * x + b3 * y + os);
        const float j = cos(a4 * x + b4 * y + os);
        const float fx = -sin(x + os);
        const float gx = -a1 * sin(a1 * x + b1 * y + os);
        const float hx = -a2 * sin(a2 * x + b2 * y + os);
        const float ix = -a3 * sin(a3 * x + b3 * y + os);
        const float jx = -a4 * sin(a4 * x + b4 * y + os);
        return -(fx * g * h * i * j + f * gx * h * i * j + f * g * hx * i * j + f * g * h * ix * j + f * g * h * i * jx);
    }
