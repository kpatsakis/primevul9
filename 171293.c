translate_ring_addresses(struct virtio_net *dev, int vq_index)
{
	struct vhost_virtqueue *vq = dev->virtqueue[vq_index];
	struct vhost_vring_addr *addr = &vq->ring_addrs;

	/* The addresses are converted from QEMU virtual to Vhost virtual. */
	if (vq->desc && vq->avail && vq->used)
		return dev;

	vq->desc = (struct vring_desc *)(uintptr_t)ring_addr_to_vva(dev,
			vq, addr->desc_user_addr, sizeof(struct vring_desc));
	if (vq->desc == 0) {
		RTE_LOG(DEBUG, VHOST_CONFIG,
			"(%d) failed to find desc ring address.\n",
			dev->vid);
		return dev;
	}

	dev = numa_realloc(dev, vq_index);
	vq = dev->virtqueue[vq_index];
	addr = &vq->ring_addrs;

	vq->avail = (struct vring_avail *)(uintptr_t)ring_addr_to_vva(dev,
			vq, addr->avail_user_addr, sizeof(struct vring_avail));
	if (vq->avail == 0) {
		RTE_LOG(DEBUG, VHOST_CONFIG,
			"(%d) failed to find avail ring address.\n",
			dev->vid);
		return dev;
	}

	vq->used = (struct vring_used *)(uintptr_t)ring_addr_to_vva(dev,
			vq, addr->used_user_addr, sizeof(struct vring_used));
	if (vq->used == 0) {
		RTE_LOG(DEBUG, VHOST_CONFIG,
			"(%d) failed to find used ring address.\n",
			dev->vid);
		return dev;
	}

	if (vq->last_used_idx != vq->used->idx) {
		RTE_LOG(WARNING, VHOST_CONFIG,
			"last_used_idx (%u) and vq->used->idx (%u) mismatches; "
			"some packets maybe resent for Tx and dropped for Rx\n",
			vq->last_used_idx, vq->used->idx);
		vq->last_used_idx  = vq->used->idx;
		vq->last_avail_idx = vq->used->idx;
	}

	vq->log_guest_addr = addr->log_guest_addr;

	LOG_DEBUG(VHOST_CONFIG, "(%d) mapped address desc: %p\n",
			dev->vid, vq->desc);
	LOG_DEBUG(VHOST_CONFIG, "(%d) mapped address avail: %p\n",
			dev->vid, vq->avail);
	LOG_DEBUG(VHOST_CONFIG, "(%d) mapped address used: %p\n",
			dev->vid, vq->used);
	LOG_DEBUG(VHOST_CONFIG, "(%d) log_guest_addr: %" PRIx64 "\n",
			dev->vid, vq->log_guest_addr);

	return dev;
}