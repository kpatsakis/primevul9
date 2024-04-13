vhost_user_set_vring_base(struct virtio_net *dev,
			  VhostUserMsg *msg)
{
	dev->virtqueue[msg->payload.state.index]->last_used_idx  =
			msg->payload.state.num;
	dev->virtqueue[msg->payload.state.index]->last_avail_idx =
			msg->payload.state.num;

	return 0;
}