static long vhost_net_reset_owner(struct vhost_net *n)
{
	struct socket *tx_sock = NULL;
	struct socket *rx_sock = NULL;
	long err;

	mutex_lock(&n->dev.mutex);
	err = vhost_dev_check_owner(&n->dev);
	if (err)
		goto done;
	vhost_net_stop(n, &tx_sock, &rx_sock);
	vhost_net_flush(n);
	err = vhost_dev_reset_owner(&n->dev);
done:
	mutex_unlock(&n->dev.mutex);
	if (tx_sock)
		fput(tx_sock->file);
	if (rx_sock)
		fput(rx_sock->file);
	return err;
}