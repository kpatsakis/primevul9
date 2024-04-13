static ssize_t queue_max_integrity_segments_show(struct request_queue *q, char *page)
{
	return queue_var_show(q->limits.max_integrity_segments, (page));
}