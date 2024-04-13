
static void ql_free_send_free_list(struct ql3_adapter *qdev)
{
	struct ql_tx_buf_cb *tx_cb;
	int i;

	tx_cb = &qdev->tx_buf[0];
	for (i = 0; i < NUM_REQ_Q_ENTRIES; i++) {
		kfree(tx_cb->oal);
		tx_cb->oal = NULL;
		tx_cb++;
	}