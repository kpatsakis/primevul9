void gd_error_ex(int priority, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	if (gd_error_method) {
		gd_error_method(priority, format, args);
	}
	va_end(args);
}