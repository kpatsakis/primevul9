vhost_user_set_vring_addr(struct virtio_net **pdev, VhostUserMsg *msg)
{
	struct vhost_virtqueue *vq;
	struct vhost_vring_addr *addr = &msg->payload.addr;
	struct virtio_net *dev = *pdev;

	if (dev->mem == NULL)
		return -1;

	/* addr->index refers to the queue index. The txq 1, rxq is 0. */
	vq = dev->virtqueue[msg->payload.addr.index];

	/*
	 * Rings addresses should not be interpreted as long as the ring is not
	 * started and enabled
	 */
	memcpy(&vq->ring_addrs, addr, sizeof(*addr));

	vring_invalidate(dev, vq);

	if (vq->enabled && (dev->features &
				(1ULL << VHOST_USER_F_PROTOCOL_FEATURES))) {
		dev = translate_ring_addresses(dev, msg->payload.addr.index);
		if (!dev)
			return -1;

		*pdev = dev;
	}

	return 0;
}