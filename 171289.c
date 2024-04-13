vhost_user_net_set_mtu(struct virtio_net *dev, struct VhostUserMsg *msg)
{
	if (msg->payload.u64 < VIRTIO_MIN_MTU ||
			msg->payload.u64 > VIRTIO_MAX_MTU) {
		RTE_LOG(ERR, VHOST_CONFIG, "Invalid MTU size (%"PRIu64")\n",
				msg->payload.u64);

		return -1;
	}

	dev->mtu = msg->payload.u64;

	return 0;
}