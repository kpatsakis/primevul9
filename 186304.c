PHPAPI void session_adapt_url(const char *url, size_t urllen, char **new, size_t *newlen) /* {{{ */
{
	if (APPLY_TRANS_SID && (PS(session_status) == php_session_active)) {
		*new = php_url_scanner_adapt_single_url(url, urllen, PS(session_name), ZSTR_VAL(PS(id)), newlen, 1);
	}
}