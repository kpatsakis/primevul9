static ssize_t queue_max_sectors_show(struct request_queue *q, char *page)
{
	int max_sectors_kb = queue_max_sectors(q) >> 1;

	return queue_var_show(max_sectors_kb, (page));
}