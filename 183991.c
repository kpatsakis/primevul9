smtp_filter_data_end(struct smtp_session *s)
{
	if (!SESSION_FILTERED(s))
		return;

	if (s->tx->filter == NULL)
		return;

	io_free(s->tx->filter);
	s->tx->filter = NULL;

	m_create(p_lka, IMSG_FILTER_SMTP_DATA_END, 0, 0, -1);
	m_add_id(p_lka, s->id);
	m_close(p_lka);
}