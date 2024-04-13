static void tx_poll_stop(struct vhost_net *net)
{
	if (likely(net->tx_poll_state != VHOST_NET_POLL_STARTED))
		return;
	vhost_poll_stop(net->poll + VHOST_NET_VQ_TX);
	net->tx_poll_state = VHOST_NET_POLL_STOPPED;
}