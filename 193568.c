static int vhost_net_open(struct inode *inode, struct file *f)
{
	struct vhost_net *n = kmalloc(sizeof *n, GFP_KERNEL);
	struct vhost_dev *dev;
	int r;

	if (!n)
		return -ENOMEM;

	dev = &n->dev;
	n->vqs[VHOST_NET_VQ_TX].handle_kick = handle_tx_kick;
	n->vqs[VHOST_NET_VQ_RX].handle_kick = handle_rx_kick;
	r = vhost_dev_init(dev, n->vqs, VHOST_NET_VQ_MAX);
	if (r < 0) {
		kfree(n);
		return r;
	}

	vhost_poll_init(n->poll + VHOST_NET_VQ_TX, handle_tx_net, POLLOUT, dev);
	vhost_poll_init(n->poll + VHOST_NET_VQ_RX, handle_rx_net, POLLIN, dev);
	n->tx_poll_state = VHOST_NET_POLL_DISABLED;

	f->private_data = n;

	return 0;
}