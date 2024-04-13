vhost_user_get_vring_base(struct virtio_net *dev,
			  VhostUserMsg *msg)
{
	struct vhost_virtqueue *vq = dev->virtqueue[msg->payload.state.index];

	/* We have to stop the queue (virtio) if it is running. */
	if (dev->flags & VIRTIO_DEV_RUNNING) {
		dev->flags &= ~VIRTIO_DEV_RUNNING;
		dev->notify_ops->destroy_device(dev->vid);
	}

	dev->flags &= ~VIRTIO_DEV_READY;

	/* Here we are safe to get the last used index */
	msg->payload.state.num = vq->last_used_idx;

	RTE_LOG(INFO, VHOST_CONFIG,
		"vring base idx:%d file:%d\n", msg->payload.state.index,
		msg->payload.state.num);
	/*
	 * Based on current qemu vhost-user implementation, this message is
	 * sent and only sent in vhost_vring_stop.
	 * TODO: cleanup the vring, it isn't usable since here.
	 */
	if (vq->kickfd >= 0)
		close(vq->kickfd);

	vq->kickfd = VIRTIO_UNINITIALIZED_EVENTFD;

	if (dev->dequeue_zero_copy)
		free_zmbufs(vq);
	rte_free(vq->shadow_used_ring);
	vq->shadow_used_ring = NULL;

	rte_free(vq->batch_copy_elems);
	vq->batch_copy_elems = NULL;

	return 0;
}