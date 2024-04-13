static ssize_t queue_poll_store(struct request_queue *q, const char *page,
				size_t count)
{
	unsigned long poll_on;
	ssize_t ret;

	if (!q->tag_set || q->tag_set->nr_maps <= HCTX_TYPE_POLL ||
	    !q->tag_set->map[HCTX_TYPE_POLL].nr_queues)
		return -EINVAL;

	ret = queue_var_store(&poll_on, page, count);
	if (ret < 0)
		return ret;

	if (poll_on)
		blk_queue_flag_set(QUEUE_FLAG_POLL, q);
	else
		blk_queue_flag_clear(QUEUE_FLAG_POLL, q);

	return ret;
}