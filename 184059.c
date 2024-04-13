smtp_check_noparam(struct smtp_session *s, const char *args)
{
	if (args != NULL) {
		smtp_reply(s, "500 %s %s: command does not accept arguments.",
		    esc_code(ESC_STATUS_PERMFAIL, ESC_INVALID_COMMAND_ARGUMENTS),
		    esc_description(ESC_INVALID_COMMAND_ARGUMENTS));
		return 0;
	}
	return 1;
}