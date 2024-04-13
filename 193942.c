static ssize_t queue_dax_show(struct request_queue *q, char *page)
{
	return queue_var_show(blk_queue_dax(q), page);
}