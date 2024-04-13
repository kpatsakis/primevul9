vhost_user_get_protocol_features(struct virtio_net *dev,
				 struct VhostUserMsg *msg)
{
	uint64_t features, protocol_features = VHOST_USER_PROTOCOL_FEATURES;

	rte_vhost_driver_get_features(dev->ifname, &features);

	/*
	 * REPLY_ACK protocol feature is only mandatory for now
	 * for IOMMU feature. If IOMMU is explicitly disabled by the
	 * application, disable also REPLY_ACK feature for older buggy
	 * Qemu versions (from v2.7.0 to v2.9.0).
	 */
	if (!(features & (1ULL << VIRTIO_F_IOMMU_PLATFORM)))
		protocol_features &= ~(1ULL << VHOST_USER_PROTOCOL_F_REPLY_ACK);

	msg->payload.u64 = protocol_features;
	msg->size = sizeof(msg->payload.u64);
}