smtp_message_begin(struct smtp_tx *tx)
{
	struct smtp_session *s;
	X509 *x;
	int	(*m_printf)(struct smtp_tx *, const char *, ...);

	m_printf = smtp_message_printf;
	if (tx->filter)
		m_printf = smtp_filter_printf;

	s = tx->session;

	log_debug("smtp: %p: message begin", s);

	smtp_reply(s, "354 Enter mail, end with \".\""
	    " on a line by itself");	
	
	if (s->junk || (s->tx && s->tx->junk))
		m_printf(tx, "X-Spam: Yes\n");

	m_printf(tx, "Received: ");
	if (!(s->listener->flags & F_MASK_SOURCE)) {
		m_printf(tx, "from %s (%s %s%s%s)",
		    s->helo,
		    s->rdns,
		    s->ss.ss_family == AF_INET6 ? "" : "[",
		    ss_to_text(&s->ss),
		    s->ss.ss_family == AF_INET6 ? "" : "]");
	}
	m_printf(tx, "\n\tby %s (%s) with %sSMTP%s%s id %08x",
	    s->smtpname,
	    SMTPD_NAME,
	    s->flags & SF_EHLO ? "E" : "",
	    s->flags & SF_SECURE ? "S" : "",
	    s->flags & SF_AUTHENTICATED ? "A" : "",
	    tx->msgid);

	if (s->flags & SF_SECURE) {
		x = SSL_get_peer_certificate(io_tls(s->io));
		m_printf(tx, " (%s:%s:%d:%s)",
		    SSL_get_version(io_tls(s->io)),
		    SSL_get_cipher_name(io_tls(s->io)),
		    SSL_get_cipher_bits(io_tls(s->io), NULL),
		    (s->flags & SF_VERIFIED) ? "YES" : (x ? "FAIL" : "NO"));
		X509_free(x);

		if (s->listener->flags & F_RECEIVEDAUTH) {
			m_printf(tx, " auth=%s",
			    s->username[0] ? "yes" : "no");
			if (s->username[0])
				m_printf(tx, " user=%s", s->username);
		}
	}

	if (tx->rcptcount == 1) {
		m_printf(tx, "\n\tfor <%s@%s>",
		    tx->evp.rcpt.user,
		    tx->evp.rcpt.domain);
	}

	m_printf(tx, ";\n\t%s\n", time_to_text(time(&tx->time)));

	smtp_enter_state(s, STATE_BODY);
}