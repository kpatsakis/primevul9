static struct ql_rcv_buf_cb *ql_get_from_lrg_buf_free_list(struct ql3_adapter
							   *qdev)
{
	struct ql_rcv_buf_cb *lrg_buf_cb = qdev->lrg_buf_free_head;

	if (lrg_buf_cb != NULL) {
		qdev->lrg_buf_free_head = lrg_buf_cb->next;
		if (qdev->lrg_buf_free_head == NULL)
			qdev->lrg_buf_free_tail = NULL;
		qdev->lrg_buf_free_count--;
	}

	return lrg_buf_cb;
}