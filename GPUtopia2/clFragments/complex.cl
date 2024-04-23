typedef float2 complex;

#define i1 ((complex)(0.f, 1.f))
#define half_pi 1.57079632679f;

inline float real(complex z) { return z.x; }
inline float imag(complex z) { return z.y; }
inline float abs(complex z) { return length(z); }
inline float carg(complex z) { return atan2(z.y, z.x); }
inline complex cabs(complex z) { return (complex)(fabs(z.x), fabs(z.y)); }

// operations with two complex arguments
// inline complex cadd(complex& z1, complex& z2) { return (complex)(z1.x + z2.x, z1.y + z2.y); }
// inline complex csub(complex& z1, complex& z2) { return (complex)(z1.x - z2.x, z1.y - z2.y); }
inline complex cmul(complex z1, complex z2) 
{ 
	return (complex)(
		z1.x * z2.x - z1.y * z2.y,
		z1.x * z2.y + z1.y * z2.x);
}
inline complex cdiv(complex z1, complex z2)
{
	const float inv_denominator = 1.f / (z2.x * z2.x + z2.y * z2.y);
	return (complex)(
		(z1.x * z2.x + z1.y * z2.y) * inv_denominator,
		(z1.y * z2.x + z1.x * z2.y) * inv_denominator);
}

// ANALYTICAL FUNCTIONS
// complex log
inline complex clog(complex z) { return (complex)(log(abs(z)),	carg(z)); }
inline complex cln(complex z) { return clog(z); }
inline complex clog2(complex z) { return 1.f / log(2.f) * clog(z); }
inline complex clog10(complex z) { return 1.f / log(10.f) * clog(z); }

// complex exp, sin, cos, tan, pow
inline complex cexp(complex z) { return exp(real(z)) * (complex)(cos(z.y), sin(z.y)); }
inline complex csin(complex z) {
	return
		-0.5f * (
			exp(-z.y) * (complex)(-sin(z.x), cos(z.x)) -
			exp(z.y) * (complex)(sin(z.x), cos(z.x)));
}
inline complex ccos(complex z) {
	z.x -= half_pi;
	return csin(z);
}
inline complex ctan(complex z) { return cdiv(cos(z), sin(z)); }
inline complex cpow(complex z1, complex z2) { return cexp(cmul(z2, clog(z1))); }
