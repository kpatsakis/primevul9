smtp_tx_create_message(struct smtp_tx *tx)
{
	m_create(p_queue, IMSG_SMTP_MESSAGE_CREATE, 0, 0, -1);
	m_add_id(p_queue, tx->session->id);
	m_close(p_queue);
	tree_xset(&wait_queue_msg, tx->session->id, tx->session);
}