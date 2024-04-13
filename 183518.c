static void _pam_log_int(const pam_handle_t *pamh,
			 int err,
			 const char *format,
			 va_list args)
{
	char *format2 = NULL;
	const char *service;

	pam_get_item(pamh, PAM_SERVICE, (const void **) &service);

	format2 = (char *)malloc(strlen(MODULE_NAME)+strlen(format)+strlen(service)+5);
	if (format2 == NULL) {
		/* what else todo ? */
		vsyslog(err, format, args);
		return;
	}

	sprintf(format2, "%s(%s): %s", MODULE_NAME, service, format);
	vsyslog(err, format2, args);
	SAFE_FREE(format2);
}