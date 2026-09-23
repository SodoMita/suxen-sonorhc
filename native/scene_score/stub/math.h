#pragma once
double sin(double x);
double cos(double x);
double exp(double x);
double exp2(double x);
double asin(double x);
double pow(double base, double exponent);
double fabs(double x);
#define isfinite(x) ((x) == (x) && (x) <= 1.7976931348623157e+308 && (x) >= -1.7976931348623157e+308)
