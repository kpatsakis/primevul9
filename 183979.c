smtp_proceed_mail_from(struct smtp_session *s, const char *args)
{
	char *copy;
	char tmp[SMTP_LINE_MAX];

	(void)strlcpy(tmp, args, sizeof tmp);
	copy = tmp;  

       	if (!smtp_tx(s)) {
		smtp_reply(s, "421 %s Temporary Error",
		    esc_code(ESC_STATUS_TEMPFAIL, ESC_OTHER_MAIL_SYSTEM_STATUS));
		smtp_enter_state(s, STATE_QUIT);
		return;
	}

	if (smtp_mailaddr(&s->tx->evp.sender, copy, 1, &copy,
		s->smtpname) == 0) {
		smtp_reply(s, "553 %s Sender address syntax error",
		    esc_code(ESC_STATUS_PERMFAIL, ESC_OTHER_ADDRESS_STATUS));
		smtp_tx_free(s->tx);
		return;
	}

	smtp_tx_mail_from(s->tx, args);
}