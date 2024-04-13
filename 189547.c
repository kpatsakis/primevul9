e_source_registry_debug_print (const gchar *format,
			       ...)
{
	va_list args;

	if (!e_source_registry_debug_enabled ())
		return;

	va_start (args, format);
	e_util_debug_printv ("ESR", format, args);
	va_end (args);
}