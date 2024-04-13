static void vhost_zerocopy_callback(struct ubuf_info *ubuf, bool success)
{
	struct vhost_ubuf_ref *ubufs = ubuf->ctx;
	struct vhost_virtqueue *vq = ubufs->vq;
	int cnt = atomic_read(&ubufs->kref.refcount);

	/*
	 * Trigger polling thread if guest stopped submitting new buffers:
	 * in this case, the refcount after decrement will eventually reach 1
	 * so here it is 2.
	 * We also trigger polling periodically after each 16 packets
	 * (the value 16 here is more or less arbitrary, it's tuned to trigger
	 * less than 10% of times).
	 */
	if (cnt <= 2 || !(cnt % 16))
		vhost_poll_queue(&vq->poll);
	/* set len to mark this desc buffers done DMA */
	vq->heads[ubuf->desc].len = success ?
		VHOST_DMA_DONE_LEN : VHOST_DMA_FAILED_LEN;
	vhost_ubuf_put(ubufs);
}