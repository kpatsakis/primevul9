static ssize_t queue_write_same_max_show(struct request_queue *q, char *page)
{
	return sprintf(page, "%llu\n",
		(unsigned long long)q->limits.max_write_same_sectors << 9);
}