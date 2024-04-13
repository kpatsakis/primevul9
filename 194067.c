static ssize_t queue_poll_delay_store(struct request_queue *q, const char *page,
				size_t count)
{
	int err, val;

	if (!q->mq_ops || !q->mq_ops->poll)
		return -EINVAL;

	err = kstrtoint(page, 10, &val);
	if (err < 0)
		return err;

	if (val == BLK_MQ_POLL_CLASSIC)
		q->poll_nsec = BLK_MQ_POLL_CLASSIC;
	else if (val >= 0)
		q->poll_nsec = val * 1000;
	else
		return -EINVAL;

	return count;
}