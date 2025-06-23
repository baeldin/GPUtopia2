__parameters:
    real parameter sineWeight = 1.;
    real parameter sineFreq = 1.;
    real parameter tanWeight = 0.01;
    real parameter tanFreq = 1.;
    complex parameter offset = (0., 0.);
    complex parameter tanOffset = (0., 0.);
__init:
//=====| fractal formula init
        complex z = z0;
__loop:
//=========| fractal formula loop
            z += (complex)(
                @sineWeight * sin(@sineFreq * z.y) + @tanWeight * tan(@tanFreq * z.y + @tanOffset.y),
                @sineWeight * sin(@sineFreq * z.x) + @tanWeight * tan(@tanFreq * z.x + @tanOffset.x));
            z += @offset;
__bailout:
//=| factal bailout function
    // must always define bailedOut!!!
    bailedOut = (dot(z, z) > bailout*bailout);
__functions:
//=| functions which are usable in the init, loop, and bailout parts