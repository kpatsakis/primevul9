static ssize_t queue_io_opt_show(struct request_queue *q, char *page)
{
	return queue_var_show(queue_io_opt(q), page);
}