static __poll_t qrtr_tun_poll(struct file *filp, poll_table *wait)
{
	struct qrtr_tun *tun = filp->private_data;
	__poll_t mask = 0;

	poll_wait(filp, &tun->readq, wait);

	if (!skb_queue_empty(&tun->queue))
		mask |= EPOLLIN | EPOLLRDNORM;

	return mask;
}