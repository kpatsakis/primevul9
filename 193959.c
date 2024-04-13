static ssize_t queue_chunk_sectors_show(struct request_queue *q, char *page)
{
	return queue_var_show(q->limits.chunk_sectors, page);
}