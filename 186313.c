static int php_session_cache_limiter(void) /* {{{ */
{
	php_session_cache_limiter_t *lim;

	if (PS(cache_limiter)[0] == '\0') return 0;
	if (PS(session_status) != php_session_active) return -1;

	if (SG(headers_sent)) {
		const char *output_start_filename = php_output_get_start_filename();
		int output_start_lineno = php_output_get_start_lineno();

		php_session_abort();
		if (output_start_filename) {
			php_error_docref(NULL, E_WARNING, "Cannot send session cache limiter - headers already sent (output started at %s:%d)", output_start_filename, output_start_lineno);
		} else {
			php_error_docref(NULL, E_WARNING, "Cannot send session cache limiter - headers already sent");
		}
		return -2;
	}

	for (lim = php_session_cache_limiters; lim->name; lim++) {
		if (!strcasecmp(lim->name, PS(cache_limiter))) {
			lim->func();
			return 0;
		}
	}

	return -1;
}