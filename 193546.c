static int vhost_net_set_features(struct vhost_net *n, u64 features)
{
	size_t vhost_hlen, sock_hlen, hdr_len;
	int i;

	hdr_len = (features & (1 << VIRTIO_NET_F_MRG_RXBUF)) ?
			sizeof(struct virtio_net_hdr_mrg_rxbuf) :
			sizeof(struct virtio_net_hdr);
	if (features & (1 << VHOST_NET_F_VIRTIO_NET_HDR)) {
		/* vhost provides vnet_hdr */
		vhost_hlen = hdr_len;
		sock_hlen = 0;
	} else {
		/* socket provides vnet_hdr */
		vhost_hlen = 0;
		sock_hlen = hdr_len;
	}
	mutex_lock(&n->dev.mutex);
	if ((features & (1 << VHOST_F_LOG_ALL)) &&
	    !vhost_log_access_ok(&n->dev)) {
		mutex_unlock(&n->dev.mutex);
		return -EFAULT;
	}
	n->dev.acked_features = features;
	smp_wmb();
	for (i = 0; i < VHOST_NET_VQ_MAX; ++i) {
		mutex_lock(&n->vqs[i].mutex);
		n->vqs[i].vhost_hlen = vhost_hlen;
		n->vqs[i].sock_hlen = sock_hlen;
		mutex_unlock(&n->vqs[i].mutex);
	}
	vhost_net_flush(n);
	mutex_unlock(&n->dev.mutex);
	return 0;
}