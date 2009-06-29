#include <stdarg.h>
#include "utils.h"

float minv(uint32_t n, ...) {
	va_list va;
	va_start(va, n);
	float min = va_arg(va, double);
	uint32_t i;
	for(i = 0; i < n - 1; i++) {
		float tmp = va_arg(va, double);
		if(min > tmp) min = tmp;
	}
	return min;
}

float maxv(uint32_t n, ...) {
	va_list va;
	va_start(va, n);
	float max = va_arg(va, double);
	uint32_t i;
	for(i = 0; i < n - 1; i++) {
		float tmp = va_arg(va, double);
		if(max < tmp) max = tmp;
	}
	return max;
}
