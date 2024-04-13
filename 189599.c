MBSTRING_API int php_unicode_is_prop(unsigned long code, ...)
{
	int result = 0;
	va_list va;
	va_start(va, code);

	while (1) {
		int prop = va_arg(va, int);
		if (prop < 0) {
			break;
		}

		if (prop_lookup(code, prop)) {
			result = 1;
			break;
		}
	}

	va_end(va);
	return result;
}