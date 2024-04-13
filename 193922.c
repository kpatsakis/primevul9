static ssize_t queue_max_segments_show(struct request_queue *q, char *page)
{
	return queue_var_show(queue_max_segments(q), (page));
}