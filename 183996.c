smtp_io(struct io *io, int evt, void *arg)
{
	struct smtp_session    *s = arg;
	char		       *line;
	size_t			len;
	int			eom;

	log_trace(TRACE_IO, "smtp: %p: %s %s", s, io_strevent(evt),
	    io_strio(io));

	switch (evt) {

	case IO_TLSREADY:
		log_info("%016"PRIx64" smtp tls ciphers=%s",
		    s->id, ssl_to_text(io_tls(s->io)));

		smtp_report_link_tls(s, ssl_to_text(io_tls(s->io)));

		s->flags |= SF_SECURE;
		s->helo[0] = '\0';

		smtp_cert_verify(s);
		break;

	case IO_DATAIN:
	    nextline:
		line = io_getline(s->io, &len);
		if ((line == NULL && io_datalen(s->io) >= SMTP_LINE_MAX) ||
		    (line && len >= SMTP_LINE_MAX)) {
			s->flags |= SF_BADINPUT;
			smtp_reply(s, "500 %s Line too long",
			    esc_code(ESC_STATUS_PERMFAIL, ESC_OTHER_STATUS));
			smtp_enter_state(s, STATE_QUIT);
			io_set_write(io);
			return;
		}

		/* No complete line received */
		if (line == NULL)
			return;

		/* Message body */
		eom = 0;
		if (s->state == STATE_BODY) {
			if (strcmp(line, ".")) {
				s->tx->datain += strlen(line) + 1;
				if (s->tx->datain > env->sc_maxsize)
					s->tx->error = TX_ERROR_SIZE;
			}
			eom = (s->tx->filter == NULL) ?
			    smtp_tx_dataline(s->tx, line) :
			    smtp_tx_filtered_dataline(s->tx, line);
			if (eom == 0)
				goto nextline;
		}

		/* Pipelining not supported */
		if (io_datalen(s->io)) {
			s->flags |= SF_BADINPUT;
			smtp_reply(s, "500 %s %s: Pipelining not supported",
			    esc_code(ESC_STATUS_PERMFAIL, ESC_INVALID_COMMAND),
			    esc_description(ESC_INVALID_COMMAND));
			smtp_enter_state(s, STATE_QUIT);
			io_set_write(io);
			return;
		}

		if (eom) {
			io_set_write(io);
			if (s->tx->filter == NULL)
				smtp_tx_eom(s->tx);
			return;
		}

		/* Must be a command */
		if (strlcpy(s->cmd, line, sizeof(s->cmd)) >= sizeof(s->cmd)) {
			s->flags |= SF_BADINPUT;
			smtp_reply(s, "500 %s Command line too long",
			    esc_code(ESC_STATUS_PERMFAIL, ESC_OTHER_STATUS));
			smtp_enter_state(s, STATE_QUIT);
			io_set_write(io);
			return;
		}
		io_set_write(io);
		smtp_command(s, line);
		break;

	case IO_LOWAT:
		if (s->state == STATE_QUIT) {
			log_info("%016"PRIx64" smtp disconnected "
			    "reason=quit",
			    s->id);
			smtp_free(s, "done");
			break;
		}

		/* Wait for the client to start tls */
		if (s->state == STATE_TLS) {
			smtp_cert_init(s);
			break;
		}

		io_set_read(io);
		break;

	case IO_TIMEOUT:
		log_info("%016"PRIx64" smtp disconnected "
		    "reason=timeout",
		    s->id);
		smtp_report_timeout(s);
		smtp_free(s, "timeout");
		break;

	case IO_DISCONNECTED:
		log_info("%016"PRIx64" smtp disconnected "
		    "reason=disconnect",
		    s->id);
		smtp_free(s, "disconnected");
		break;

	case IO_ERROR:
		log_info("%016"PRIx64" smtp disconnected "
		    "reason=\"io-error: %s\"",
		    s->id, io_error(io));
		smtp_free(s, "IO error");
		break;

	default:
		fatalx("smtp_io()");
	}
}