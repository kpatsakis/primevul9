smtp_session_imsg(struct mproc *p, struct imsg *imsg)
{
	struct smtp_session		*s;
	struct smtp_rcpt		*rcpt;
	char				 user[SMTPD_MAXMAILADDRSIZE];
	char				 tmp[SMTP_LINE_MAX];
	struct msg			 m;
	const char			*line, *helo;
	uint64_t			 reqid, evpid;
	uint32_t			 msgid;
	int				 status, success;
	int                              filter_response;
	const char                      *filter_param;
	uint8_t                          i;

	switch (imsg->hdr.type) {

	case IMSG_SMTP_CHECK_SENDER:
		m_msg(&m, imsg);
		m_get_id(&m, &reqid);
		m_get_int(&m, &status);
		m_end(&m);
		s = tree_xpop(&wait_lka_mail, reqid);
		switch (status) {
		case LKA_OK:
			smtp_tx_create_message(s->tx);
			break;

		case LKA_PERMFAIL:
			smtp_tx_free(s->tx);
			smtp_reply(s, "%d %s", 530, "Sender rejected");
			break;
		case LKA_TEMPFAIL:
			smtp_tx_free(s->tx);
			smtp_reply(s, "421 %s Temporary Error",
			    esc_code(ESC_STATUS_TEMPFAIL, ESC_OTHER_MAIL_SYSTEM_STATUS));
			break;
		}
		return;

	case IMSG_SMTP_EXPAND_RCPT:
		m_msg(&m, imsg);
		m_get_id(&m, &reqid);
		m_get_int(&m, &status);
		m_get_string(&m, &line);
		m_end(&m);
		s = tree_xpop(&wait_lka_rcpt, reqid);

		tmp[0] = '\0';
		if (s->tx->evp.rcpt.user[0]) {
			(void)strlcpy(tmp, s->tx->evp.rcpt.user, sizeof tmp);
			if (s->tx->evp.rcpt.domain[0]) {
				(void)strlcat(tmp, "@", sizeof tmp);
				(void)strlcat(tmp, s->tx->evp.rcpt.domain,
				    sizeof tmp);
			}
		}

		switch (status) {
		case LKA_OK:
			fatalx("unexpected ok");
		case LKA_PERMFAIL:
			smtp_reply(s, "%s: <%s>", line, tmp);
			break;
		case LKA_TEMPFAIL:
			smtp_reply(s, "%s: <%s>", line, tmp);
			break;
		}
		return;

	case IMSG_SMTP_LOOKUP_HELO:
		m_msg(&m, imsg);
		m_get_id(&m, &reqid);
		s = tree_xpop(&wait_lka_helo, reqid);
		m_get_int(&m, &status);
		if (status == LKA_OK) {
			m_get_string(&m, &helo);
			(void)strlcpy(s->smtpname, helo, sizeof(s->smtpname));
		}
		m_end(&m);
		smtp_connected(s);
		return;

	case IMSG_SMTP_MESSAGE_CREATE:
		m_msg(&m, imsg);
		m_get_id(&m, &reqid);
		m_get_int(&m, &success);
		s = tree_xpop(&wait_queue_msg, reqid);
		if (success) {
			m_get_msgid(&m, &msgid);
			s->tx->msgid = msgid;
			s->tx->evp.id = msgid_to_evpid(msgid);
			s->tx->rcptcount = 0;
			smtp_reply(s, "250 %s Ok",
			    esc_code(ESC_STATUS_OK, ESC_OTHER_STATUS));
		} else {
			smtp_reply(s, "421 %s Temporary Error",
			    esc_code(ESC_STATUS_TEMPFAIL, ESC_OTHER_MAIL_SYSTEM_STATUS));
			smtp_tx_free(s->tx);
			smtp_enter_state(s, STATE_QUIT);
		}
		m_end(&m);
		return;

	case IMSG_SMTP_MESSAGE_OPEN:
		m_msg(&m, imsg);
		m_get_id(&m, &reqid);
		m_get_int(&m, &success);
		m_end(&m);

		s = tree_xpop(&wait_queue_fd, reqid);
		if (!success || imsg->fd == -1) {
			if (imsg->fd != -1)
				close(imsg->fd);
			smtp_reply(s, "421 %s Temporary Error",
			    esc_code(ESC_STATUS_TEMPFAIL, ESC_OTHER_MAIL_SYSTEM_STATUS));
			smtp_enter_state(s, STATE_QUIT);
			return;
		}

		log_debug("smtp: %p: fd %d from queue", s, imsg->fd);

		if (smtp_message_fd(s->tx, imsg->fd)) {
			if (!SESSION_DATA_FILTERED(s))
				smtp_message_begin(s->tx);
			else
				smtp_filter_data_begin(s);
		}
		return;

	case IMSG_FILTER_SMTP_DATA_BEGIN:
		m_msg(&m, imsg);
		m_get_id(&m, &reqid);
		m_get_int(&m, &success);
		m_end(&m);

		s = tree_xpop(&wait_filter_fd, reqid);
		if (!success || imsg->fd == -1) {
			if (imsg->fd != -1)
				close(imsg->fd);
			smtp_reply(s, "421 %s Temporary Error",
			    esc_code(ESC_STATUS_TEMPFAIL, ESC_OTHER_MAIL_SYSTEM_STATUS));
			smtp_enter_state(s, STATE_QUIT);
			return;
		}

		log_debug("smtp: %p: fd %d from lka", s, imsg->fd);

		smtp_filter_fd(s->tx, imsg->fd);
		smtp_message_begin(s->tx);
		return;

	case IMSG_QUEUE_ENVELOPE_SUBMIT:
		m_msg(&m, imsg);
		m_get_id(&m, &reqid);
		m_get_int(&m, &success);
		s = tree_xget(&wait_lka_rcpt, reqid);
		if (success) {
			m_get_evpid(&m, &evpid);
			s->tx->evp.id = evpid;
			s->tx->destcount++;
			smtp_report_tx_envelope(s, s->tx->msgid, evpid);
		}
		else
			s->tx->error = TX_ERROR_ENVELOPE;
		m_end(&m);
		return;

	case IMSG_QUEUE_ENVELOPE_COMMIT:
		m_msg(&m, imsg);
		m_get_id(&m, &reqid);
		m_get_int(&m, &success);
		m_end(&m);
		if (!success)
			fatalx("commit evp failed: not supposed to happen");
		s = tree_xpop(&wait_lka_rcpt, reqid);
		if (s->tx->error) {
			/*
			 * If an envelope failed, we can't cancel the last
			 * RCPT only so we must cancel the whole transaction
			 * and close the connection.
			 */
			smtp_reply(s, "421 %s Temporary failure",
			    esc_code(ESC_STATUS_TEMPFAIL, ESC_OTHER_MAIL_SYSTEM_STATUS));
			smtp_enter_state(s, STATE_QUIT);
		}
		else {
			rcpt = xcalloc(1, sizeof(*rcpt));
			rcpt->evpid = s->tx->evp.id;
			rcpt->destcount = s->tx->destcount;
			rcpt->maddr = s->tx->evp.rcpt;
			TAILQ_INSERT_TAIL(&s->tx->rcpts, rcpt, entry);

			s->tx->destcount = 0;
			s->tx->rcptcount++;
			smtp_reply(s, "250 %s %s: Recipient ok",
			    esc_code(ESC_STATUS_OK, ESC_DESTINATION_ADDRESS_VALID),
			    esc_description(ESC_DESTINATION_ADDRESS_VALID));
		}
		return;

	case IMSG_SMTP_MESSAGE_COMMIT:
		m_msg(&m, imsg);
		m_get_id(&m, &reqid);
		m_get_int(&m, &success);
		m_end(&m);
		s = tree_xpop(&wait_queue_commit, reqid);
		if (!success) {
			smtp_reply(s, "421 %s Temporary failure",
			    esc_code(ESC_STATUS_TEMPFAIL, ESC_OTHER_MAIL_SYSTEM_STATUS));
			smtp_tx_free(s->tx);
			smtp_enter_state(s, STATE_QUIT);
			return;
		}

		smtp_reply(s, "250 %s %08x Message accepted for delivery",
		    esc_code(ESC_STATUS_OK, ESC_OTHER_STATUS),
		    s->tx->msgid);
		smtp_report_tx_commit(s, s->tx->msgid, s->tx->odatalen);
		smtp_report_tx_reset(s, s->tx->msgid);

		log_info("%016"PRIx64" smtp message "
		    "msgid=%08x size=%zu nrcpt=%zu proto=%s",
		    s->id,
		    s->tx->msgid,
		    s->tx->odatalen,
		    s->tx->rcptcount,
		    s->flags & SF_EHLO ? "ESMTP" : "SMTP");
		TAILQ_FOREACH(rcpt, &s->tx->rcpts, entry) {
			log_info("%016"PRIx64" smtp envelope "
			    "evpid=%016"PRIx64" from=<%s%s%s> to=<%s%s%s>",
			    s->id,
			    rcpt->evpid,
			    s->tx->evp.sender.user,
			    s->tx->evp.sender.user[0] == '\0' ? "" : "@",
			    s->tx->evp.sender.domain,
			    rcpt->maddr.user,
			    rcpt->maddr.user[0] == '\0' ? "" : "@",
			    rcpt->maddr.domain);
		}
		smtp_tx_free(s->tx);
		s->mailcount++;
		smtp_enter_state(s, STATE_HELO);
		return;

	case IMSG_SMTP_AUTHENTICATE:
		m_msg(&m, imsg);
		m_get_id(&m, &reqid);
		m_get_int(&m, &success);
		m_end(&m);

		s = tree_xpop(&wait_parent_auth, reqid);
		strnvis(user, s->username, sizeof user, VIS_WHITE | VIS_SAFE);
		if (success == LKA_OK) {
			log_info("%016"PRIx64" smtp "
			    "authentication user=%s "
			    "result=ok",
			    s->id, user);
			s->flags |= SF_AUTHENTICATED;
			smtp_report_link_auth(s, user, "pass");
			smtp_reply(s, "235 %s Authentication succeeded",
			    esc_code(ESC_STATUS_OK, ESC_OTHER_STATUS));
		}
		else if (success == LKA_PERMFAIL) {
			log_info("%016"PRIx64" smtp "
			    "authentication user=%s "
			    "result=permfail",
			    s->id, user);
			smtp_report_link_auth(s, user, "fail");
			smtp_auth_failure_pause(s);
			return;
		}
		else if (success == LKA_TEMPFAIL) {
			log_info("%016"PRIx64" smtp "
			    "authentication user=%s "
			    "result=tempfail",
			    s->id, user);
			smtp_report_link_auth(s, user, "error");
			smtp_reply(s, "421 %s Temporary failure",
			    esc_code(ESC_STATUS_TEMPFAIL, ESC_OTHER_MAIL_SYSTEM_STATUS));
		}
		else
			fatalx("bad lka response");

		smtp_enter_state(s, STATE_HELO);
		return;

	case IMSG_FILTER_SMTP_PROTOCOL:
		m_msg(&m, imsg);
		m_get_id(&m, &reqid);
		m_get_int(&m, &filter_response);
		if (filter_response != FILTER_PROCEED &&
		    filter_response != FILTER_JUNK)
			m_get_string(&m, &filter_param);
		else
			filter_param = NULL;
		m_end(&m);

		s = tree_xpop(&wait_filters, reqid);

		switch (filter_response) {
		case FILTER_REJECT:
		case FILTER_DISCONNECT:
			if (!valid_smtp_response(filter_param) ||
			    (filter_param[0] != '4' && filter_param[0] != '5'))
				filter_param = "421 Internal server error";
			if (!strncmp(filter_param, "421", 3))
				filter_response = FILTER_DISCONNECT;

			smtp_report_filter_response(s, s->filter_phase,
			    filter_response, filter_param);

			smtp_reply(s, "%s", filter_param);

			if (filter_response == FILTER_DISCONNECT)
				smtp_enter_state(s, STATE_QUIT);
			else if (s->filter_phase == FILTER_COMMIT)
				smtp_proceed_rollback(s, NULL);
			break;


		case FILTER_JUNK:
			if (s->tx)
				s->tx->junk = 1;
			else
				s->junk = 1;
			/* fallthrough */

		case FILTER_PROCEED:
			filter_param = s->filter_param;
			/* fallthrough */

		case FILTER_REWRITE:
			smtp_report_filter_response(s, s->filter_phase,
			    filter_response,
			    filter_param == s->filter_param ? NULL : filter_param);
			if (s->filter_phase == FILTER_CONNECT) {
				smtp_proceed_connected(s);
				return;
			}
			for (i = 0; i < nitems(commands); ++i)
				if (commands[i].filter_phase == s->filter_phase) {
					if (filter_response == FILTER_REWRITE)
						if (!commands[i].check(s, filter_param))
							break;
					commands[i].proceed(s, filter_param);
					break;
				}
			break;
		}
		return;
	}

	log_warnx("smtp_session_imsg: unexpected %s imsg",
	    imsg_to_str(imsg->hdr.type));
	fatalx(NULL);
}