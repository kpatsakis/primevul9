smtp_filter_begin(struct smtp_session *s)
{
	if (!SESSION_FILTERED(s))
		return;

	m_create(p_lka, IMSG_FILTER_SMTP_BEGIN, 0, 0, -1);
	m_add_id(p_lka, s->id);
	m_add_string(p_lka, s->listener->filter_name);
	m_close(p_lka);
}