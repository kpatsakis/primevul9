static ssize_t queue_discard_zeroes_data_show(struct request_queue *q, char *page)
{
	return queue_var_show(0, page);
}