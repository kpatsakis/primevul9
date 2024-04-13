vhost_user_set_vring_enable(struct virtio_net *dev,
			    VhostUserMsg *msg)
{
	int enable = (int)msg->payload.state.num;

	RTE_LOG(INFO, VHOST_CONFIG,
		"set queue enable: %d to qp idx: %d\n",
		enable, msg->payload.state.index);

	if (dev->notify_ops->vring_state_changed)
		dev->notify_ops->vring_state_changed(dev->vid,
				msg->payload.state.index, enable);

	dev->virtqueue[msg->payload.state.index]->enabled = enable;

	return 0;
}