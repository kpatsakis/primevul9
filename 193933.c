static ssize_t queue_wc_store(struct request_queue *q, const char *page,
			      size_t count)
{
	int set = -1;

	if (!strncmp(page, "write back", 10))
		set = 1;
	else if (!strncmp(page, "write through", 13) ||
		 !strncmp(page, "none", 4))
		set = 0;

	if (set == -1)
		return -EINVAL;

	if (set)
		blk_queue_flag_set(QUEUE_FLAG_WC, q);
	else
		blk_queue_flag_clear(QUEUE_FLAG_WC, q);

	return count;
}