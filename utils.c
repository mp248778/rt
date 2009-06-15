float minv(unsigned n, ...) {
	va_list va;
	va_start(va, n);
	float min = va_arg(va, float);
	unsigned il
	for(i = 0; i < n - 1; i++) {
		float tmp = va_arg(va, float);
		if(min > tmp) min = tmp;
	}
	return min;
}

float maxv(unsigned n, ...) {
	va_list va;
	va_start(va, n);
	float max = va_arg(va, float);
	unsigned il
	for(i = 0; i < n - 1; i++) {
		float tmp = va_arg(va, float);
		if(max < tmp) max = tmp;
	}
	return max;
}
