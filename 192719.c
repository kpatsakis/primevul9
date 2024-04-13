static inline int php_math_is_finite(double value) {
#if defined(PHP_WIN32)
	return _finite(value);
#elif defined(isfinite)
	return isfinite(value);
#else
	return value == value && (value == 0. || value * 2. != value);
#endif
}