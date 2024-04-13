smtp_check_rset(struct smtp_session *s, const char *args)
{
	if (!smtp_check_noparam(s, args))
		return 0;

	if (s->helo[0] == '\0') {
		smtp_reply(s, "503 %s %s: Command not allowed at this point.",
		    esc_code(ESC_STATUS_PERMFAIL, ESC_INVALID_COMMAND),
		    esc_description(ESC_INVALID_COMMAND));
		return 0;
	}
	return 1;
}