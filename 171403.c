
static void ql_init_large_buffers(struct ql3_adapter *qdev)
{
	int i;
	struct ql_rcv_buf_cb *lrg_buf_cb;
	struct bufq_addr_element *buf_addr_ele = qdev->lrg_buf_q_virt_addr;

	for (i = 0; i < qdev->num_large_buffers; i++) {
		lrg_buf_cb = &qdev->lrg_buf[i];
		buf_addr_ele->addr_high = lrg_buf_cb->buf_phy_addr_high;
		buf_addr_ele->addr_low = lrg_buf_cb->buf_phy_addr_low;
		buf_addr_ele++;
	}
	qdev->lrg_buf_index = 0;
	qdev->lrg_buf_skb_check = 0;