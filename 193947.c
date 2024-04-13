static ssize_t queue_nr_zones_show(struct request_queue *q, char *page)
{
	return queue_var_show(blk_queue_nr_zones(q), page);
}