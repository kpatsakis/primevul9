smtp_tx_open_message(struct smtp_tx *tx)
{
	m_create(p_queue, IMSG_SMTP_MESSAGE_OPEN, 0, 0, -1);
	m_add_id(p_queue, tx->session->id);
	m_add_msgid(p_queue, tx->msgid);
	m_close(p_queue);
	tree_xset(&wait_queue_fd, tx->session->id, tx->session);
}