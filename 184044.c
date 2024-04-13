smtp_proceed_help(struct smtp_session *s, const char *args)
{
	const char *code = esc_code(ESC_STATUS_OK, ESC_OTHER_STATUS);

	smtp_reply(s, "214-%s This is " SMTPD_NAME, code);
	smtp_reply(s, "214-%s To report bugs in the implementation, "
	    "please contact bugs@openbsd.org", code);
	smtp_reply(s, "214-%s with full details", code);
	smtp_reply(s, "214 %s End of HELP info", code);
}