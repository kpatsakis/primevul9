e_util_debug_print (const gchar *domain,
		    const gchar *format,
		    ...)
{
	va_list args;

	va_start (args, format);
	e_util_debug_printv (domain, format, args);
	va_end (args);
}