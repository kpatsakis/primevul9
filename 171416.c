
static int ql_create_send_free_list(struct ql3_adapter *qdev)
{
	struct ql_tx_buf_cb *tx_cb;
	int i;
	struct ob_mac_iocb_req *req_q_curr = qdev->req_q_virt_addr;

	/* Create free list of transmit buffers */
	for (i = 0; i < NUM_REQ_Q_ENTRIES; i++) {

		tx_cb = &qdev->tx_buf[i];
		tx_cb->skb = NULL;
		tx_cb->queue_entry = req_q_curr;
		req_q_curr++;
		tx_cb->oal = kmalloc(512, GFP_KERNEL);
		if (tx_cb->oal == NULL)
			return -ENOMEM;
	}
	return 0;