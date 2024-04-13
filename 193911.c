static ssize_t queue_max_hw_sectors_show(struct request_queue *q, char *page)
{
	int max_hw_sectors_kb = queue_max_hw_sectors(q) >> 1;

	return queue_var_show(max_hw_sectors_kb, (page));
}