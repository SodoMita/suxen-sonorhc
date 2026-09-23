#pragma once
typedef unsigned long size_t;
typedef int wchar_t;
typedef long ptrdiff_t;
typedef long long intmax_t;
#define NULL ((void *)0)
#define offsetof(type, member) ((size_t) & ((type *)0)->member)
