smtp_tx_dataline(struct smtp_tx *tx, const char *line)
{
	struct rfc5322_result res;
	int r;

	log_trace(TRACE_SMTP, "<<< [MSG] %s", line);

	if (!strcmp(line, ".")) {
		smtp_report_protocol_client(tx->session, ".");
		log_trace(TRACE_SMTP, "<<< [EOM]");
		if (tx->error)
			return 1;
		line = NULL;
	}
	else {
		/* ignore data line if an error is set */
		if (tx->error)
			return 0;

		/* escape lines starting with a '.' */
		if (line[0] == '.')
			line += 1;
	}

	if (rfc5322_push(tx->parser, line) == -1) {
		log_warnx("failed to push dataline");
		tx->error = TX_ERROR_INTERNAL;
		return 0;
	}

	for(;;) {
		r = rfc5322_next(tx->parser, &res);
		switch (r) {
		case -1:
			if (errno == ENOMEM)
				tx->error = TX_ERROR_INTERNAL;
			else
				tx->error = TX_ERROR_MALFORMED;
			return 0;

		case RFC5322_NONE:
			/* Need more data */
			return 0;

		case RFC5322_HEADER_START:
			/* ignore bcc */
			if (!strcasecmp("Bcc", res.hdr))
				continue;

			if (!strcasecmp("To", res.hdr) ||
			    !strcasecmp("Cc", res.hdr) ||
			    !strcasecmp("From", res.hdr)) {
				rfc5322_unfold_header(tx->parser);
				continue;
			}

			if (!strcasecmp("Received", res.hdr)) {
				if (++tx->rcvcount >= MAX_HOPS_COUNT) {
					log_warnx("warn: loop detected");
					tx->error = TX_ERROR_LOOP;
					return 0;
				}
			}
			else if (!tx->has_date && !strcasecmp("Date", res.hdr))
				tx->has_date = 1;
			else if (!tx->has_message_id &&
			    !strcasecmp("Message-Id", res.hdr))
				tx->has_message_id = 1;

			smtp_message_printf(tx, "%s:%s\n", res.hdr, res.value);
			break;

		case RFC5322_HEADER_CONT:

			if (!strcasecmp("Bcc", res.hdr) ||
			    !strcasecmp("To", res.hdr) ||
			    !strcasecmp("Cc", res.hdr) ||
			    !strcasecmp("From", res.hdr))
				continue;

			smtp_message_printf(tx, "%s\n", res.value);
			break;

		case RFC5322_HEADER_END:
			if (!strcasecmp("To", res.hdr) ||
			    !strcasecmp("Cc", res.hdr) ||
			    !strcasecmp("From", res.hdr))
				header_domain_append_callback(tx, res.hdr,
				    res.value);
			break;

		case RFC5322_END_OF_HEADERS:
			if (tx->session->listener->local ||
			    tx->session->listener->port == 587) {

				if (!tx->has_date) {
					log_debug("debug: %p: adding Date", tx);
					smtp_message_printf(tx, "Date: %s\n",
					    time_to_text(tx->time));
				}

				if (!tx->has_message_id) {
					log_debug("debug: %p: adding Message-ID", tx);
					smtp_message_printf(tx,
					    "Message-ID: <%016"PRIx64"@%s>\n",
					    generate_uid(),
					    tx->session->listener->hostname);
				}
			}
			break;

		case RFC5322_BODY_START:
		case RFC5322_BODY:
			smtp_message_printf(tx, "%s\n", res.value);
			break;

		case RFC5322_END_OF_MESSAGE:
			return 1;

		default:
			fatalx("%s", __func__);
		}
	}
}