smtp_lookup_servername(struct smtp_session *s)
{
	if (s->listener->hostnametable[0]) {
		m_create(p_lka, IMSG_SMTP_LOOKUP_HELO, 0, 0, -1);
		m_add_id(p_lka, s->id);
		m_add_string(p_lka, s->listener->hostnametable);
		m_add_sockaddr(p_lka, (struct sockaddr*)&s->listener->ss);
		m_close(p_lka);
		tree_xset(&wait_lka_helo, s->id, s);
		return;
	}

	smtp_connected(s);
}