virtio_is_ready(struct virtio_net *dev)
{
	struct vhost_virtqueue *vq;
	uint32_t i;

	if (dev->nr_vring == 0)
		return 0;

	for (i = 0; i < dev->nr_vring; i++) {
		vq = dev->virtqueue[i];

		if (!vq_is_ready(vq))
			return 0;
	}

	RTE_LOG(INFO, VHOST_CONFIG,
		"virtio is now ready for processing.\n");
	return 1;
}