static ssize_t queue_max_discard_segments_show(struct request_queue *q,
		char *page)
{
	return queue_var_show(queue_max_discard_segments(q), (page));
}