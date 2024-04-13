smtp_filter_data_begin(struct smtp_session *s)
{
	if (!SESSION_FILTERED(s))
		return;

	m_create(p_lka, IMSG_FILTER_SMTP_DATA_BEGIN, 0, 0, -1);
	m_add_id(p_lka, s->id);
	m_close(p_lka);
	tree_xset(&wait_filter_fd, s->id, s);
}