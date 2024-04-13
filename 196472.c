static __poll_t rpmsg_eptdev_poll(struct file *filp, poll_table *wait)
{
	struct rpmsg_eptdev *eptdev = filp->private_data;
	__poll_t mask = 0;

	if (!eptdev->ept)
		return EPOLLERR;

	poll_wait(filp, &eptdev->readq, wait);

	if (!skb_queue_empty(&eptdev->queue))
		mask |= EPOLLIN | EPOLLRDNORM;

	mask |= rpmsg_poll(eptdev->ept, filp, wait);

	return mask;
}