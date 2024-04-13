static ssize_t queue_requests_show(struct request_queue *q, char *page)
{
	return queue_var_show(q->nr_requests, (page));
}