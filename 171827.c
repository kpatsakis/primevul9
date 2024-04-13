static void _pam_log(struct pwb_context *r, int err, const char *format, ...)
{
	va_list args;

	if (_pam_log_is_silent(r->ctrl)) {
		return;
	}

	va_start(args, format);
	_pam_log_int(r->pamh, err, format, args);
	va_end(args);
}