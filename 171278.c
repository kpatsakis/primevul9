vhost_user_set_vring_kick(struct virtio_net **pdev, struct VhostUserMsg *pmsg)
{
	struct vhost_vring_file file;
	struct vhost_virtqueue *vq;
	struct virtio_net *dev = *pdev;

	file.index = pmsg->payload.u64 & VHOST_USER_VRING_IDX_MASK;
	if (pmsg->payload.u64 & VHOST_USER_VRING_NOFD_MASK)
		file.fd = VIRTIO_INVALID_EVENTFD;
	else
		file.fd = pmsg->fds[0];
	RTE_LOG(INFO, VHOST_CONFIG,
		"vring kick idx:%d file:%d\n", file.index, file.fd);

	/* Interpret ring addresses only when ring is started. */
	dev = translate_ring_addresses(dev, file.index);
	if (!dev)
		return;

	*pdev = dev;

	vq = dev->virtqueue[file.index];

	/*
	 * When VHOST_USER_F_PROTOCOL_FEATURES is not negotiated,
	 * the ring starts already enabled. Otherwise, it is enabled via
	 * the SET_VRING_ENABLE message.
	 */
	if (!(dev->features & (1ULL << VHOST_USER_F_PROTOCOL_FEATURES)))
		vq->enabled = 1;

	if (vq->kickfd >= 0)
		close(vq->kickfd);
	vq->kickfd = file.fd;
}