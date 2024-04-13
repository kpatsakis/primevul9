static ssize_t queue_physical_block_size_show(struct request_queue *q, char *page)
{
	return queue_var_show(queue_physical_block_size(q), page);
}