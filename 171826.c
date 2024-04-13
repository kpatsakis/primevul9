static void __pam_log_debug(const pam_handle_t *pamh, int ctrl, int err, const char *format, ...)
{
	va_list args;

	if (!_pam_log_is_debug_enabled(ctrl)) {
		return;
	}

	va_start(args, format);
	_pam_log_int(pamh, err, format, args);
	va_end(args);
}