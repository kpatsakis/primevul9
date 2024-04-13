smtp_tx_rcpt_to(struct smtp_tx *tx, const char *line)
{
	char *opt, *p;
	char *copy;
	char tmp[SMTP_LINE_MAX];

	(void)strlcpy(tmp, line, sizeof tmp);
	copy = tmp; 

	if (tx->rcptcount >= env->sc_session_max_rcpt) {
		smtp_reply(tx->session, "451 %s %s: Too many recipients",
		    esc_code(ESC_STATUS_TEMPFAIL, ESC_TOO_MANY_RECIPIENTS),
		    esc_description(ESC_TOO_MANY_RECIPIENTS));
		return;
	}

	if (smtp_mailaddr(&tx->evp.rcpt, copy, 0, &copy,
	    tx->session->smtpname) == 0) {
		smtp_reply(tx->session,
		    "501 %s Recipient address syntax error",
		    esc_code(ESC_STATUS_PERMFAIL,
		        ESC_BAD_DESTINATION_MAILBOX_ADDRESS_SYNTAX));
		return;
	}

	while ((opt = strsep(&copy, " "))) {
		if (*opt == '\0')
			continue;

		if (ADVERTISE_EXT_DSN(tx->session) && strncasecmp(opt, "NOTIFY=", 7) == 0) {
			opt += 7;
			while ((p = strsep(&opt, ","))) {
				if (strcasecmp(p, "SUCCESS") == 0)
					tx->evp.dsn_notify |= DSN_SUCCESS;
				else if (strcasecmp(p, "FAILURE") == 0)
					tx->evp.dsn_notify |= DSN_FAILURE;
				else if (strcasecmp(p, "DELAY") == 0)
					tx->evp.dsn_notify |= DSN_DELAY;
				else if (strcasecmp(p, "NEVER") == 0)
					tx->evp.dsn_notify |= DSN_NEVER;
			}

			if (tx->evp.dsn_notify & DSN_NEVER &&
			    tx->evp.dsn_notify & (DSN_SUCCESS | DSN_FAILURE |
			    DSN_DELAY)) {
				smtp_reply(tx->session,
				    "553 NOTIFY option NEVER cannot be"
				    " combined with other options");
				return;
			}
		} else if (ADVERTISE_EXT_DSN(tx->session) && strncasecmp(opt, "ORCPT=", 6) == 0) {
			opt += 6;
			if (!text_to_mailaddr(&tx->evp.dsn_orcpt, opt)) {
				smtp_reply(tx->session,
				    "553 ORCPT address syntax error");
				return;
			}
		} else {
			smtp_reply(tx->session, "503 Unsupported option %s", opt);
			return;
		}
	}

	m_create(p_lka, IMSG_SMTP_EXPAND_RCPT, 0, 0, -1);
	m_add_id(p_lka, tx->session->id);
	m_add_envelope(p_lka, &tx->evp);
	m_close(p_lka);
	tree_xset(&wait_lka_rcpt, tx->session->id, tx->session);
}