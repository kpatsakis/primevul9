static void vhost_net_disable_vq(struct vhost_net *n,
				 struct vhost_virtqueue *vq)
{
	if (!vq->private_data)
		return;
	if (vq == n->vqs + VHOST_NET_VQ_TX) {
		tx_poll_stop(n);
		n->tx_poll_state = VHOST_NET_POLL_DISABLED;
	} else
		vhost_poll_stop(n->poll + VHOST_NET_VQ_RX);
}