static void vhost_net_stop(struct vhost_net *n, struct socket **tx_sock,
			   struct socket **rx_sock)
{
	*tx_sock = vhost_net_stop_vq(n, n->vqs + VHOST_NET_VQ_TX);
	*rx_sock = vhost_net_stop_vq(n, n->vqs + VHOST_NET_VQ_RX);
}