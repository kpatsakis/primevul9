static int vhost_zerocopy_signal_used(struct vhost_net *net,
				      struct vhost_virtqueue *vq)
{
	int i;
	int j = 0;

	for (i = vq->done_idx; i != vq->upend_idx; i = (i + 1) % UIO_MAXIOV) {
		if (vq->heads[i].len == VHOST_DMA_FAILED_LEN)
			vhost_net_tx_err(net);
		if (VHOST_DMA_IS_DONE(vq->heads[i].len)) {
			vq->heads[i].len = VHOST_DMA_CLEAR_LEN;
			vhost_add_used_and_signal(vq->dev, vq,
						  vq->heads[i].id, 0);
			++j;
		} else
			break;
	}
	if (j)
		vq->done_idx = i;
	return j;
}