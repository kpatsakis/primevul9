smtp_tx_rollback(struct smtp_tx *tx)
{
	m_create(p_queue, IMSG_SMTP_MESSAGE_ROLLBACK, 0, 0, -1);
	m_add_msgid(p_queue, tx->msgid);
	m_close(p_queue);
	smtp_report_tx_rollback(tx->session, tx->msgid);
	smtp_report_tx_reset(tx->session, tx->msgid);
	smtp_filter_data_end(tx->session);
}