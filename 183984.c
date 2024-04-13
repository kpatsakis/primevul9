smtp_check_mail_from(struct smtp_session *s, const char *args)
{
	char *copy;
	char tmp[SMTP_LINE_MAX];
	struct mailaddr	sender;

	(void)strlcpy(tmp, args, sizeof tmp);
	copy = tmp;  

	if (s->helo[0] == '\0' || s->tx) {
		smtp_reply(s, "503 %s %s: Command not allowed at this point.",
		    esc_code(ESC_STATUS_PERMFAIL, ESC_INVALID_COMMAND),
		    esc_description(ESC_INVALID_COMMAND));
		return 0;
	}

	if (s->listener->flags & F_STARTTLS_REQUIRE &&
	    !(s->flags & SF_SECURE)) {
		smtp_reply(s,
		    "530 %s %s: Must issue a STARTTLS command first",
		    esc_code(ESC_STATUS_PERMFAIL, ESC_INVALID_COMMAND),
		    esc_description(ESC_INVALID_COMMAND));
		return 0;
	}

	if (s->listener->flags & F_AUTH_REQUIRE &&
	    !(s->flags & SF_AUTHENTICATED)) {
		smtp_reply(s,
		    "530 %s %s: Must issue an AUTH command first",
		    esc_code(ESC_STATUS_PERMFAIL, ESC_INVALID_COMMAND),
		    esc_description(ESC_INVALID_COMMAND));
		return 0;
	}

	if (s->mailcount >= env->sc_session_max_mails) {
		/* we can pretend we had too many recipients */
		smtp_reply(s, "452 %s %s: Too many messages sent",
		    esc_code(ESC_STATUS_TEMPFAIL, ESC_TOO_MANY_RECIPIENTS),
		    esc_description(ESC_TOO_MANY_RECIPIENTS));
		return 0;
	}

	if (smtp_mailaddr(&sender, copy, 1, &copy,
		s->smtpname) == 0) {
		smtp_reply(s, "553 %s Sender address syntax error",
		    esc_code(ESC_STATUS_PERMFAIL, ESC_OTHER_ADDRESS_STATUS));
		return 0;
	}

	return 1;
}