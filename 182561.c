write_config_option_secret (int fd, const char *key, const char *value)
{
	gs_free char *string = NULL;
	int x;

	string = g_strdup_printf ("%s %s\n", key, value);

	x = write (fd, string, strlen (string));
	if (x < 0)
		_LOGW ("Unexpected error in write(): %d", errno);

	_LOGD ("Config: %s <hidden>", key);
}