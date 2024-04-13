static ssize_t qrtr_tun_read_iter(struct kiocb *iocb, struct iov_iter *to)
{
	struct file *filp = iocb->ki_filp;
	struct qrtr_tun *tun = filp->private_data;
	struct sk_buff *skb;
	int count;

	while (!(skb = skb_dequeue(&tun->queue))) {
		if (filp->f_flags & O_NONBLOCK)
			return -EAGAIN;

		/* Wait until we get data or the endpoint goes away */
		if (wait_event_interruptible(tun->readq,
					     !skb_queue_empty(&tun->queue)))
			return -ERESTARTSYS;
	}

	count = min_t(size_t, iov_iter_count(to), skb->len);
	if (copy_to_iter(skb->data, count, to) != count)
		count = -EFAULT;

	kfree_skb(skb);

	return count;
}