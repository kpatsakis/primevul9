static int tw5864_start_streaming(struct vb2_queue *q, unsigned int count)
{
	struct tw5864_input *input = vb2_get_drv_priv(q);
	int ret;

	ret = tw5864_enable_input(input);
	if (!ret)
		return 0;

	while (!list_empty(&input->active)) {
		struct tw5864_buf *buf = list_entry(input->active.next,
						    struct tw5864_buf, list);

		list_del(&buf->list);
		vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_QUEUED);
	}
	return ret;
}