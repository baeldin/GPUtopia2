#pragma once

#include <cmath>
#include <iostream>

#include "json.hpp"
using json = nlohmann::ordered_json;

using std::ostream;
using std::basic_ostream;
using std::cout;
// using namespace std;

template <class T>
class Complex
{
public:
	T x;
	T y;
	Complex() : x(0), y(0) {}
	Complex(T realPart) : x(realPart), y(0) {}
	Complex(T realPart, T imagPart) : x(realPart), y(imagPart) {}
	constexpr Complex flip() const { return Complex<T>(y, x); }
	constexpr Complex conj() const { return Complex<T>(x, -y); }
	constexpr double abs_squared() const { return (double)x * x + (double)y * y; }
	constexpr double cabs() const { return sqrt(x * x + y * y); }
	constexpr Complex abs() const { return Complex<T>(std::abs(x), std::abs(y)); }
	constexpr Complex round() const { return Complex<T>(std::round(x), std::round(y)); }
	constexpr double angle() const { return std::atan2(y, x); }
	//TODO: can I const this somehow?
	template <typename T>
	friend ostream& operator<<(ostream& os, const Complex<T>& z);
	Complex operator= (const Complex& rhs) {
		x = rhs.x;
		y = rhs.y;
		return *this;
	}
	constexpr Complex operator+ (const Complex& rhs) const {
		return Complex(x + rhs.x, y + rhs.y);
	}
	constexpr Complex operator+ (const double& rhs) const {
		return Complex(x + rhs, y);
	}
	constexpr Complex operator- (const double& rhs) const {
		return Complex(x - rhs, y);
	}
	constexpr Complex operator- (const Complex& rhs) const {
		return Complex(x - rhs.x, y - rhs.y);
	}
	constexpr Complex operator* (const Complex<T>& rhs) const {
		return Complex<T>(x * rhs.x - y * rhs.y, x * rhs.y + y * rhs.x);
	}
	constexpr Complex operator* (const double& rhs) const {
		return Complex(x * rhs, y * rhs);
	}
	constexpr Complex operator/ (const double& rhs) const {
		double inv = 1 / rhs;
		return Complex(x * inv, y * inv);
	}
	constexpr Complex operator/ (const Complex& rhs) const {
		const double rhsInvAbsSqr = 1. / rhs.abs_squared();
		return Complex(x * rhs.x + y * rhs.y, y * rhs.x - x * rhs.y) * rhsInvAbsSqr;
	}
	constexpr Complex<double> exp() const {
		return Complex<double>(std::cos(y), std::sin(y)) * std::exp(x);
	}
	constexpr Complex<double> ln() const {
		return Complex<double>(std::log(this->cabs()), std::atan2(y, x));
	}
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(Complex, x, y);
};

template <typename T>
inline const bool operator==(const Complex<T> lhs, const Complex<T> rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

template <typename T>
inline const bool operator!=(const Complex<T> lhs, const Complex<T> rhs)
{
	return !(rhs == lhs);
}

template <typename T>
inline const Complex<T>& operator/ (const T lhs, const Complex<T>& rhs) {
	Complex<T> clhs(lhs);
	return clhs / rhs;
}

template <typename T>
inline const Complex<T>& operator* (const T lhs, const Complex<T>& rhs) {
	return rhs * lhs;
}

template <typename T>
inline const T cabs(const Complex<T>& z)
{
	return sqrt(z.x * z.x + z.y * z.y);
}

template <typename T>
inline const Complex<T> abs(const Complex<T>& z)
{
	return Complex<T>(std::abs(z.x), std::abs(z.y));
}

template <typename T> // real base, complex exponent
inline const Complex<T> exp(const Complex<T>& z) {
	return Complex<T>(std::cos(z.y), std::sin(z.y)) * std::exp(z.x);
}

template <typename T> // real base, complex exponent
inline const Complex<T> ln(const Complex<T>& z) {
	return Complex<T>(std::log(z.cabs()), std::atan2(z.y, z.x));
}

template <typename T> // real base, complex exponent
inline const Complex<T>& pow(const T& lhs, const Complex<T>& rhs)
{
	const Complex<T> z = Complex<T>(rhs.x, rhs.y) * std::log(lhs);
	//cout << z << "\n";
	return z.exp();
}

template <typename T> // complex base, real exponent
inline const Complex<double>& pow(const Complex<T>& lhs, const T& rhs)
{
	return Complex<T>(std::cos(lhs.angle() * rhs), std::sin(lhs.angle() * rhs)) * std::pow(lhs.cabs(), rhs);
}

template <typename T> // complex base, complex exponent
inline const Complex<T>& pow(const Complex<T>& lhs, const Complex<T>& rhs)
{
	const Complex<double> z = rhs * lhs.ln();
	//cout << z << "\n";
	return z.exp();
}

inline const Complex i1(0., 1.);
inline const Complex invi(0., -1.);

template <typename T> // real base, complex exponent
inline const Complex<T> sin(const Complex<T>& z) {
	return Complex<T>(0.5 * invi * (exp(i1 * z) - exp(invi * z)));
}

template <typename T> // real base, complex exponent
inline const Complex<T> cos(const Complex<T>& z) {
	return Complex<T>(0.5 * (exp(i1 * z) + exp(invi * z)));
}

template <typename T>
inline ostream& operator<<(ostream& os, const Complex<T>& z)
{
	const char* sign = (z.y >= 0) ? "+" : "";
	os << z.x << sign << z.y << "i";
	return os;
}

