smtp_tx_mail_from(struct smtp_tx *tx, const char *line)
{
	char *opt;
	char *copy;
	char tmp[SMTP_LINE_MAX];

	(void)strlcpy(tmp, line, sizeof tmp);
	copy = tmp;  

	if (smtp_mailaddr(&tx->evp.sender, copy, 1, &copy,
		tx->session->smtpname) == 0) {
		smtp_reply(tx->session, "553 %s Sender address syntax error",
		    esc_code(ESC_STATUS_PERMFAIL, ESC_OTHER_ADDRESS_STATUS));
		smtp_tx_free(tx);
		return;
	}

	while ((opt = strsep(&copy, " "))) {
		if (*opt == '\0')
			continue;

		if (strncasecmp(opt, "AUTH=", 5) == 0)
			log_debug("debug: smtp: AUTH in MAIL FROM command");
		else if (strncasecmp(opt, "SIZE=", 5) == 0)
			log_debug("debug: smtp: SIZE in MAIL FROM command");
		else if (strcasecmp(opt, "BODY=7BIT") == 0)
			/* XXX only for this transaction */
			tx->session->flags &= ~SF_8BITMIME;
		else if (strcasecmp(opt, "BODY=8BITMIME") == 0)
			;
		else if (ADVERTISE_EXT_DSN(tx->session) && strncasecmp(opt, "RET=", 4) == 0) {
			opt += 4;
			if (strcasecmp(opt, "HDRS") == 0)
				tx->evp.dsn_ret = DSN_RETHDRS;
			else if (strcasecmp(opt, "FULL") == 0)
				tx->evp.dsn_ret = DSN_RETFULL;
		} else if (ADVERTISE_EXT_DSN(tx->session) && strncasecmp(opt, "ENVID=", 6) == 0) {
			opt += 6;
			if (strlcpy(tx->evp.dsn_envid, opt, sizeof(tx->evp.dsn_envid))
			    >= sizeof(tx->evp.dsn_envid)) {
				smtp_reply(tx->session,
				    "503 %s %s: option too large, truncated: %s",
				    esc_code(ESC_STATUS_PERMFAIL, ESC_INVALID_COMMAND_ARGUMENTS),
				    esc_description(ESC_INVALID_COMMAND_ARGUMENTS), opt);
				smtp_tx_free(tx);
				return;
			}
		} else {
			smtp_reply(tx->session, "503 %s %s: Unsupported option %s",
			    esc_code(ESC_STATUS_PERMFAIL, ESC_INVALID_COMMAND_ARGUMENTS),
			    esc_description(ESC_INVALID_COMMAND_ARGUMENTS), opt);
			smtp_tx_free(tx);
			return;
		}
	}

	/* only check sendertable if defined and user has authenticated */
	if (tx->session->flags & SF_AUTHENTICATED &&
	    tx->session->listener->sendertable[0]) {
		m_create(p_lka, IMSG_SMTP_CHECK_SENDER, 0, 0, -1);
		m_add_id(p_lka, tx->session->id);
		m_add_string(p_lka, tx->session->listener->sendertable);
		m_add_string(p_lka, tx->session->username);
		m_add_mailaddr(p_lka, &tx->evp.sender);
		m_close(p_lka);
		tree_xset(&wait_lka_mail, tx->session->id, tx->session);
	}
	else
		smtp_tx_create_message(tx);
}