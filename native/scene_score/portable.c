/* Tiny libc for targets whose SDK is not available (Android bionic).
 * Accurate enough for the score's pitch and envelopes. Not a general libm.
 */
#include <stddef.h>
#include <stdint.h>

void *memset(void *dest, int value, size_t n) {
	unsigned char *out = (unsigned char *)dest;
	size_t i;
	for (i = 0; i < n; i++) {
		out[i] = (unsigned char)value;
	}
	return dest;
}

void *memcpy(void *dest, const void *src, size_t n) {
	unsigned char *out = (unsigned char *)dest;
	const unsigned char *in = (const unsigned char *)src;
	size_t i;
	for (i = 0; i < n; i++) {
		out[i] = in[i];
	}
	return dest;
}

void *memmove(void *dest, const void *src, size_t n) {
	unsigned char *out = (unsigned char *)dest;
	const unsigned char *in = (const unsigned char *)src;
	size_t i;
	if (out == in || n == 0) {
		return dest;
	}
	if (out < in) {
		for (i = 0; i < n; i++) {
			out[i] = in[i];
		}
	} else {
		for (i = n; i > 0; i--) {
			out[i - 1] = in[i - 1];
		}
	}
	return dest;
}

static double wrap_pi(double x) {
	const double pi = 3.14159265358979323846;
	const double two_pi = 6.28318530717958647692;
	if (x > pi || x < -pi) {
		double turns = x / two_pi;
		turns = turns >= 0.0 ? (double)(long long)(turns + 0.5) : (double)(long long)(turns - 0.5);
		x -= turns * two_pi;
	}
	if (x > pi) {
		x -= two_pi;
	} else if (x < -pi) {
		x += two_pi;
	}
	return x;
}

double sin(double x) {
	double x2;
	x = wrap_pi(x);
	x2 = x * x;
	return x * (1.0 - x2 * (1.0 / 6.0 - x2 * (1.0 / 120.0 - x2 * (1.0 / 5040.0 - x2 / 362880.0))));
}

double cos(double x) {
	double x2;
	x = wrap_pi(x);
	x2 = x * x;
	return 1.0 - x2 * (0.5 - x2 * (1.0 / 24.0 - x2 * (1.0 / 720.0 - x2 / 40320.0)));
}

double exp2(double x) {
	union {
		double d;
		uint64_t u;
	} bits;
	double n;
	double f;
	double f2;
	int exp_bits;
	if (x > 1023.0) {
		return 1e300;
	}
	if (x < -1022.0) {
		return 0.0;
	}
	n = (double)(long long)x;
	if (n > x) {
		n -= 1.0;
	}
	f = x - n;
	f2 = f * f;
	f = 1.0 + f * 0.6931471805599453 + f2 * (0.2402265069591007 + f * (0.05550410866482158 + f * (0.009618129107628477 + f * 0.001333355814642844)));
	bits.d = f;
	exp_bits = (int)((bits.u >> 52) & 0x7ff) + (int)n;
	if (exp_bits >= 0x7ff) {
		return 1e300;
	}
	if (exp_bits <= 0) {
		return 0.0;
	}
	bits.u = (bits.u & 0x800fffffffffffffULL) | ((uint64_t)exp_bits << 52);
	return bits.d;
}

double exp(double x) {
	return exp2(x * 1.4426950408889634);
}

double asin(double x) {
	double x2;
	int neg = 0;
	if (x < 0.0) {
		neg = 1;
		x = -x;
	}
	if (x > 1.0) {
		x = 1.0;
	}
	x2 = x * x;
	x = x + x * x2 * (0.166666666666666 + x2 * (0.075 + x2 * (0.044642857142857 + x2 * 0.030381944444444)));
	if (x > 0.92) {
		x = 1.5707963267948966 - (1.0 - x) * (1.0 + (1.0 - x) * 0.5);
	}
	return neg ? -x : x;
}

double fabs(double x) {
	return x < 0.0 ? -x : x;
}

static double log2_approx(double x) {
	int exp_bits;
	double m;
	double y;
	union {
		double d;
		uint64_t u;
	} bits;
	if (x <= 0.0) {
		return -1022.0;
	}
	bits.d = x;
	exp_bits = (int)((bits.u >> 52) & 0x7ff) - 1023;
	bits.u = (bits.u & 0x000fffffffffffffULL) | 0x3ff0000000000000ULL;
	m = bits.d;
	y = m - 1.0;
	return (double)exp_bits + y * (1.4426950408889634 - y * (0.7213475204444817 - y * 0.320598246518585));
}

double pow(double base, double exponent) {
	if (base == 2.0) {
		return exp2(exponent);
	}
	if (base <= 0.0) {
		return 0.0;
	}
	return exp2(exponent * log2_approx(base));
}
