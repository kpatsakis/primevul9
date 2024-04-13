static ssize_t queue_nomerges_show(struct request_queue *q, char *page)
{
	return queue_var_show((blk_queue_nomerges(q) << 1) |
			       blk_queue_noxmerges(q), page);
}