static void vhost_net_flush(struct vhost_net *n)
{
	vhost_net_flush_vq(n, VHOST_NET_VQ_TX);
	vhost_net_flush_vq(n, VHOST_NET_VQ_RX);
	if (n->dev.vqs[VHOST_NET_VQ_TX].ubufs) {
		mutex_lock(&n->dev.vqs[VHOST_NET_VQ_TX].mutex);
		n->tx_flush = true;
		mutex_unlock(&n->dev.vqs[VHOST_NET_VQ_TX].mutex);
		/* Wait for all lower device DMAs done. */
		vhost_ubuf_put_and_wait(n->dev.vqs[VHOST_NET_VQ_TX].ubufs);
		mutex_lock(&n->dev.vqs[VHOST_NET_VQ_TX].mutex);
		n->tx_flush = false;
		kref_init(&n->dev.vqs[VHOST_NET_VQ_TX].ubufs->kref);
		mutex_unlock(&n->dev.vqs[VHOST_NET_VQ_TX].mutex);
	}
}