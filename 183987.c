smtp_check_starttls(struct smtp_session *s, const char *args)
{
	if (s->helo[0] == '\0' || s->tx) {
		smtp_reply(s, "503 %s %s: Command not allowed at this point.",
		    esc_code(ESC_STATUS_PERMFAIL, ESC_INVALID_COMMAND),
		    esc_description(ESC_INVALID_COMMAND));
		return 0;
	}

	if (!(s->listener->flags & F_STARTTLS)) {
		smtp_reply(s, "503 %s %s: Command not supported",
		    esc_code(ESC_STATUS_PERMFAIL, ESC_INVALID_COMMAND),
		    esc_description(ESC_INVALID_COMMAND));
		return 0;
	}

	if (s->flags & SF_SECURE) {
		smtp_reply(s, "503 %s %s: Channel already secured",
		    esc_code(ESC_STATUS_PERMFAIL, ESC_INVALID_COMMAND),
		    esc_description(ESC_INVALID_COMMAND));
		return 0;
	}

	if (args != NULL) {
		smtp_reply(s, "501 %s %s: No parameters allowed",
		    esc_code(ESC_STATUS_PERMFAIL, ESC_INVALID_COMMAND_ARGUMENTS),
		    esc_description(ESC_INVALID_COMMAND_ARGUMENTS));
		return 0;
	}

	return 1;
}