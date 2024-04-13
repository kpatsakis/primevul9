vhost_user_set_features(struct virtio_net *dev, uint64_t features)
{
	uint64_t vhost_features = 0;

	rte_vhost_driver_get_features(dev->ifname, &vhost_features);
	if (features & ~vhost_features) {
		RTE_LOG(ERR, VHOST_CONFIG,
			"(%d) received invalid negotiated features.\n",
			dev->vid);
		return -1;
	}

	if (dev->flags & VIRTIO_DEV_RUNNING) {
		if (dev->features == features)
			return 0;

		/*
		 * Error out if master tries to change features while device is
		 * in running state. The exception being VHOST_F_LOG_ALL, which
		 * is enabled when the live-migration starts.
		 */
		if ((dev->features ^ features) & ~(1ULL << VHOST_F_LOG_ALL)) {
			RTE_LOG(ERR, VHOST_CONFIG,
				"(%d) features changed while device is running.\n",
				dev->vid);
			return -1;
		}

		if (dev->notify_ops->features_changed)
			dev->notify_ops->features_changed(dev->vid, features);
	}

	dev->features = features;
	if (dev->features &
		((1 << VIRTIO_NET_F_MRG_RXBUF) | (1ULL << VIRTIO_F_VERSION_1))) {
		dev->vhost_hlen = sizeof(struct virtio_net_hdr_mrg_rxbuf);
	} else {
		dev->vhost_hlen = sizeof(struct virtio_net_hdr);
	}
	LOG_DEBUG(VHOST_CONFIG,
		"(%d) mergeable RX buffers %s, virtio 1 %s\n",
		dev->vid,
		(dev->features & (1 << VIRTIO_NET_F_MRG_RXBUF)) ? "on" : "off",
		(dev->features & (1ULL << VIRTIO_F_VERSION_1)) ? "on" : "off");

	if ((dev->flags & VIRTIO_DEV_BUILTIN_VIRTIO_NET) &&
	    !(dev->features & (1ULL << VIRTIO_NET_F_MQ))) {
		/*
		 * Remove all but first queue pair if MQ hasn't been
		 * negotiated. This is safe because the device is not
		 * running at this stage.
		 */
		while (dev->nr_vring > 2) {
			struct vhost_virtqueue *vq;

			vq = dev->virtqueue[--dev->nr_vring];
			if (!vq)
				continue;

			dev->virtqueue[dev->nr_vring] = NULL;
			cleanup_vq(vq, 1);
			free_vq(vq);
		}
	}

	return 0;
}