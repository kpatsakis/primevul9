smtp_reply(struct smtp_session *s, char *fmt, ...)
{
	va_list	 ap;
	int	 n;
	char	 buf[LINE_MAX*2], tmp[LINE_MAX*2];

	va_start(ap, fmt);
	n = vsnprintf(buf, sizeof buf, fmt, ap);
	va_end(ap);
	if (n < 0)
		fatalx("smtp_reply: response format error");
	if (n < 4)
		fatalx("smtp_reply: response too short");
	if (n >= (int)sizeof buf) {
		/* only first three bytes are used by SMTP logic,
		 * so if _our_ reply does not fit entirely in the
		 * buffer, it's ok to truncate.
		 */
	}

	log_trace(TRACE_SMTP, "smtp: %p: >>> %s", s, buf);
	smtp_report_protocol_server(s, buf);

	switch (buf[0]) {
	case '2':
		if (s->tx) {
			if (s->last_cmd == CMD_MAIL_FROM) {
				smtp_report_tx_begin(s, s->tx->msgid);
				smtp_report_tx_mail(s, s->tx->msgid, s->cmd + 10, 1);
			}
			else if (s->last_cmd == CMD_RCPT_TO)
				smtp_report_tx_rcpt(s, s->tx->msgid, s->cmd + 8, 1);
		}
		break;
	case '3':
		if (s->tx) {
			if (s->last_cmd == CMD_DATA)
				smtp_report_tx_data(s, s->tx->msgid, 1);
		}
		break;
	case '5':
	case '4':
		/* do not report smtp_tx_mail/smtp_tx_rcpt errors
		 * if they happened outside of a transaction.
		 */
		if (s->tx) {
			if (s->last_cmd == CMD_MAIL_FROM)
				smtp_report_tx_mail(s, s->tx->msgid,
				    s->cmd + 10, buf[0] == '4' ? -1 : 0);
			else if (s->last_cmd == CMD_RCPT_TO)
				smtp_report_tx_rcpt(s,
				    s->tx->msgid, s->cmd + 8, buf[0] == '4' ? -1 : 0);
			else if (s->last_cmd == CMD_DATA && s->tx->rcptcount)
				smtp_report_tx_data(s, s->tx->msgid,
				    buf[0] == '4' ? -1 : 0);
		}

		if (s->flags & SF_BADINPUT) {
			log_info("%016"PRIx64" smtp "
			    "bad-input result=\"%.*s\"",
			    s->id, n, buf);
		}
		else if (s->state == STATE_AUTH_INIT) {
			log_info("%016"PRIx64" smtp "
			    "failed-command "
			    "command=\"AUTH PLAIN (...)\" result=\"%.*s\"",
			    s->id, n, buf);
		}
		else if (s->state == STATE_AUTH_USERNAME) {
			log_info("%016"PRIx64" smtp "
			    "failed-command "
			    "command=\"AUTH LOGIN (username)\" result=\"%.*s\"",
			    s->id, n, buf);
		}
		else if (s->state == STATE_AUTH_PASSWORD) {
			log_info("%016"PRIx64" smtp "
			    "failed-command "
			    "command=\"AUTH LOGIN (password)\" result=\"%.*s\"",
			    s->id, n, buf);
		}
		else {
			strnvis(tmp, s->cmd, sizeof tmp, VIS_SAFE | VIS_CSTYLE);
			log_info("%016"PRIx64" smtp "
			    "failed-command command=\"%s\" "
			    "result=\"%.*s\"",
			    s->id, tmp, n, buf);
		}
		break;
	}

	io_xprintf(s->io, "%s\r\n", buf);
}