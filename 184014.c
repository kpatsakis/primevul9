smtp_check_helo(struct smtp_session *s, const char *args)
{
	if (!s->banner_sent) {
		smtp_reply(s, "503 %s %s: Command not allowed at this point.",
		    esc_code(ESC_STATUS_PERMFAIL, ESC_INVALID_COMMAND),
		    esc_description(ESC_INVALID_COMMAND));
		return 0;
	}

	if (s->helo[0]) {
		smtp_reply(s, "503 %s %s: Already identified",
		    esc_code(ESC_STATUS_PERMFAIL, ESC_INVALID_COMMAND),
		    esc_description(ESC_INVALID_COMMAND));
		return 0;
	}

	if (args == NULL) {
		smtp_reply(s, "501 %s %s: HELO requires domain name",
		    esc_code(ESC_STATUS_PERMFAIL, ESC_INVALID_COMMAND),
		    esc_description(ESC_INVALID_COMMAND));
		return 0;
	}

	if (!valid_domainpart(args)) {
		smtp_reply(s, "501 %s %s: Invalid domain name",
		    esc_code(ESC_STATUS_PERMFAIL, ESC_INVALID_COMMAND_ARGUMENTS),
		    esc_description(ESC_INVALID_COMMAND_ARGUMENTS));
		return 0;
	}

	return 1;
}