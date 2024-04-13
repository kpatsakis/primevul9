
static struct ql_rcv_buf_cb *ql_get_lbuf(struct ql3_adapter *qdev)
{
	struct ql_rcv_buf_cb *lrg_buf_cb = NULL;
	lrg_buf_cb = &qdev->lrg_buf[qdev->lrg_buf_index];
	qdev->lrg_buf_release_cnt++;
	if (++qdev->lrg_buf_index == qdev->num_large_buffers)
		qdev->lrg_buf_index = 0;
	return lrg_buf_cb;