smtp_check_rcpt_to(struct smtp_session *s, const char *args)
{
	char *copy;
	char tmp[SMTP_LINE_MAX];

	(void)strlcpy(tmp, args, sizeof tmp);
	copy = tmp; 

	if (s->tx == NULL) {
		smtp_reply(s, "503 %s %s: Command not allowed at this point.",
		    esc_code(ESC_STATUS_PERMFAIL, ESC_INVALID_COMMAND),
		    esc_description(ESC_INVALID_COMMAND));
		return 0;
	}

	if (s->tx->rcptcount >= env->sc_session_max_rcpt) {
		smtp_reply(s->tx->session, "451 %s %s: Too many recipients",
		    esc_code(ESC_STATUS_TEMPFAIL, ESC_TOO_MANY_RECIPIENTS),
		    esc_description(ESC_TOO_MANY_RECIPIENTS));
		return 0;
	}

	if (smtp_mailaddr(&s->tx->evp.rcpt, copy, 0, &copy,
		s->tx->session->smtpname) == 0) {
		smtp_reply(s->tx->session,
		    "501 %s Recipient address syntax error",
		    esc_code(ESC_STATUS_PERMFAIL,
		        ESC_BAD_DESTINATION_MAILBOX_ADDRESS_SYNTAX));
		return 0;
	}

	return 1;
}