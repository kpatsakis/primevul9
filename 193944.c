static ssize_t queue_discard_max_store(struct request_queue *q,
				       const char *page, size_t count)
{
	unsigned long max_discard;
	ssize_t ret = queue_var_store(&max_discard, page, count);

	if (ret < 0)
		return ret;

	if (max_discard & (q->limits.discard_granularity - 1))
		return -EINVAL;

	max_discard >>= 9;
	if (max_discard > UINT_MAX)
		return -EINVAL;

	if (max_discard > q->limits.max_hw_discard_sectors)
		max_discard = q->limits.max_hw_discard_sectors;

	q->limits.max_discard_sectors = max_discard;
	return ret;
}