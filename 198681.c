static void tw5864_stop_streaming(struct vb2_queue *q)
{
	unsigned long flags;
	struct tw5864_input *input = vb2_get_drv_priv(q);

	tw5864_disable_input(input);

	spin_lock_irqsave(&input->slock, flags);
	if (input->vb) {
		vb2_buffer_done(&input->vb->vb.vb2_buf, VB2_BUF_STATE_ERROR);
		input->vb = NULL;
	}
	while (!list_empty(&input->active)) {
		struct tw5864_buf *buf = list_entry(input->active.next,
						    struct tw5864_buf, list);

		list_del(&buf->list);
		vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_ERROR);
	}
	spin_unlock_irqrestore(&input->slock, flags);
}