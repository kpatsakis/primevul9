smtp_tx_commit(struct smtp_tx *tx)
{
	m_create(p_queue, IMSG_SMTP_MESSAGE_COMMIT, 0, 0, -1);
	m_add_id(p_queue, tx->session->id);
	m_add_msgid(p_queue, tx->msgid);
	m_close(p_queue);
	tree_xset(&wait_queue_commit, tx->session->id, tx->session);
	smtp_filter_data_end(tx->session);
}