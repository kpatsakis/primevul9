_tiff_error_handler (const char *mod,
		     const char *fmt,
		     va_list     ap)
{
	if (G_UNLIKELY (_tiff_error))
		return;

	_tiff_error = g_strdup_vprintf (fmt, ap);
}