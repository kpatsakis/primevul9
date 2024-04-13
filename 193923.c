static ssize_t queue_io_timeout_store(struct request_queue *q, const char *page,
				  size_t count)
{
	unsigned int val;
	int err;

	err = kstrtou32(page, 10, &val);
	if (err || val == 0)
		return -EINVAL;

	blk_queue_rq_timeout(q, msecs_to_jiffies(val));

	return count;
}