smtp_proceed_auth(struct smtp_session *s, const char *args)
{
	char tmp[SMTP_LINE_MAX];
	char *eom, *method;

	(void)strlcpy(tmp, args, sizeof tmp);

	method = tmp;
	eom = strchr(tmp, ' ');
	if (eom == NULL)
		eom = strchr(tmp, '\t');
	if (eom != NULL)
		*eom++ = '\0';
	if (strcasecmp(method, "PLAIN") == 0)
		smtp_rfc4954_auth_plain(s, eom);
	else if (strcasecmp(method, "LOGIN") == 0)
		smtp_rfc4954_auth_login(s, eom);
	else
		smtp_reply(s, "504 %s %s: AUTH method \"%s\" not supported",
		    esc_code(ESC_STATUS_PERMFAIL, ESC_SECURITY_FEATURES_NOT_SUPPORTED),
		    esc_description(ESC_SECURITY_FEATURES_NOT_SUPPORTED),
		    method);
}