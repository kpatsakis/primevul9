smtp_check_auth(struct smtp_session *s, const char *args)
{
	if (s->helo[0] == '\0' || s->tx) {
		smtp_reply(s, "503 %s %s: Command not allowed at this point.",
		    esc_code(ESC_STATUS_PERMFAIL, ESC_INVALID_COMMAND),
		    esc_description(ESC_INVALID_COMMAND));
		return 0;
	}

	if (s->flags & SF_AUTHENTICATED) {
		smtp_reply(s, "503 %s %s: Already authenticated",
		    esc_code(ESC_STATUS_PERMFAIL, ESC_INVALID_COMMAND),
		    esc_description(ESC_INVALID_COMMAND));
		return 0;
	}

	if (!ADVERTISE_AUTH(s)) {
		smtp_reply(s, "503 %s %s: Command not supported",
		    esc_code(ESC_STATUS_PERMFAIL, ESC_INVALID_COMMAND),
		    esc_description(ESC_INVALID_COMMAND));
		return 0;
	}

	if (args == NULL) {
		smtp_reply(s, "501 %s %s: No parameters given",
		    esc_code(ESC_STATUS_PERMFAIL, ESC_INVALID_COMMAND_ARGUMENTS),
		    esc_description(ESC_INVALID_COMMAND_ARGUMENTS));
		return 0;
	}

	return 1;
}