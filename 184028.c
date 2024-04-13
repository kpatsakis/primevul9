smtp_filter_end(struct smtp_session *s)
{
	if (!SESSION_FILTERED(s))
		return;

	m_create(p_lka, IMSG_FILTER_SMTP_END, 0, 0, -1);
	m_add_id(p_lka, s->id);
	m_close(p_lka);
}