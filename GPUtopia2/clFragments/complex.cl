
typedef real2 complex;
// typedef float2 complex;

#define i1 ((complex)(0, 1))
#define half_pi 1.570796326794896619231321691639751442;

inline real re(const complex z) { return z.x; }
inline real im(const complex z) { return z.y; }
inline real abs(const complex z) { return length(z); }
inline real carg(const complex z) { return atan2(z.y, z.x); }
inline real angle(const complex z) { return atan2(z.y, z.x); }
inline complex flip(const complex z) { return (complex)(z.y, z.x); }
inline complex conj(const complex z) { return (complex)(z.x, -z.y); }
inline complex abs_squared(const complex z) { return (complex)(z.x * z.x, z.y * z.y); }
inline complex cabs(const complex z) { return (complex)(absf(z.x), absf(z.y)); }

// operations with two complex arguments
inline complex cmul(const complex z1, const complex z2) 
{ 
	return (complex)(
		z1.x * z2.x - z1.y * z2.y,
		z1.x * z2.y + z1.y * z2.x);
}
inline complex cdiv(const complex z1, const complex z2)
{
	const real inv_denominator = 1.f / (z2.x * z2.x + z2.y * z2.y);
	return (complex)(
		(z1.x * z2.x + z1.y * z2.y) * inv_denominator,
		(z1.y * z2.x - z1.x * z2.y) * inv_denominator);
}

// ANALYTICAL FUNCTIONS
// complex log
inline complex clog(const complex z) { return (complex)(log(abs(z)), carg(z)); }
inline complex cln(const complex z) { return clog(z); }
inline complex clog2(const complex z) { return 1.f / log(2.f) * clog(z); }
inline complex clog10(const complex z) { return 1.f / log(10.f) * clog(z); }

// complex exp, sin, cos, tan, pow
inline complex cexp(const complex z) { return exp(z.x) * (complex)(cos(z.y), sin(z.y)); }
inline complex csin(const complex z) {
	return
		-0.5f * (
			exp(-z.y) * (complex)(-sin(z.x), cos(z.x)) -
			exp(z.y) * (complex)(sin(z.x), cos(z.x)));
}
inline complex ccos(complex z) {
	z.x -= half_pi;
	return csin(z);
}
inline complex ctan(const complex z) { return cdiv(cos(z), sin(z)); }

// pow functions for complex exponents and bases
inline complex __attribute__((overloadable)) cpow(const real x, const complex z)
{
	const real logx = log(x);
	return cexp((complex)(z.x * logx, z.y * logx));
}
inline complex __attribute__((overloadable)) cpow(const complex z, const real x)
{
	const real powxz = pow(abs(z), x);
	return (complex)(cos(angle(z) * x) * powxz,	sin(angle(z) * x) * powxz);
}
inline complex __attribute__((overloadable)) cpow(complex z1, complex z2) { return cexp(cmul(z2, clog(z1))); }
