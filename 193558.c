static int vhost_net_release(struct inode *inode, struct file *f)
{
	struct vhost_net *n = f->private_data;
	struct socket *tx_sock;
	struct socket *rx_sock;

	vhost_net_stop(n, &tx_sock, &rx_sock);
	vhost_net_flush(n);
	vhost_dev_stop(&n->dev);
	vhost_dev_cleanup(&n->dev, false);
	if (tx_sock)
		fput(tx_sock->file);
	if (rx_sock)
		fput(rx_sock->file);
	/* We do an extra flush before freeing memory,
	 * since jobs can re-queue themselves. */
	vhost_net_flush(n);
	kfree(n);
	return 0;
}