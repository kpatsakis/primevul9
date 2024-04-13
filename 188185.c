void gd_error(const char *format, ...)
{
	va_list args;

	va_start(args, format);
	gd_error_ex(GD_WARNING, format, args);
	va_end(args);
}