smtp_query_filters(enum filter_phase phase, struct smtp_session *s, const char *args)
{
	m_create(p_lka, IMSG_FILTER_SMTP_PROTOCOL, 0, 0, -1);
	m_add_id(p_lka, s->id);
	m_add_int(p_lka, phase);
	m_add_string(p_lka, args);
	m_close(p_lka);
	tree_xset(&wait_filters, s->id, s);
}