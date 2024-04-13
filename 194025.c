static ssize_t queue_discard_granularity_show(struct request_queue *q, char *page)
{
	return queue_var_show(q->limits.discard_granularity, page);
}