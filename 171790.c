static void _pam_log_int(const pam_handle_t *pamh,
			 int err,
			 const char *format,
			 va_list args)
{
	pam_vsyslog(pamh, err, format, args);
}