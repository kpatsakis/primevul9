static void vhost_net_enable_vq(struct vhost_net *n,
				struct vhost_virtqueue *vq)
{
	struct socket *sock;

	sock = rcu_dereference_protected(vq->private_data,
					 lockdep_is_held(&vq->mutex));
	if (!sock)
		return;
	if (vq == n->vqs + VHOST_NET_VQ_TX) {
		n->tx_poll_state = VHOST_NET_POLL_STOPPED;
		tx_poll_start(n, sock);
	} else
		vhost_poll_start(n->poll + VHOST_NET_VQ_RX, sock->file);
}