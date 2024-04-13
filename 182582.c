write_config_option (int fd, const char *format, ...)
{
	gs_free char *string = NULL;
	va_list args;
	int x;

	va_start (args, format);
	string = g_strdup_vprintf (format, args);
	va_end (args);

	x = write (fd, string, strlen (string));
	if (x < 0)
		_LOGW ("Unexpected error in write(): %d", errno);
	x = write (fd, "\n", 1);
	if (x < 0)
		_LOGW ("Unexpected error in write(): %d", errno);

	_LOGD ("Config: %s", string);
}