static ssize_t queue_write_zeroes_max_show(struct request_queue *q, char *page)
{
	return sprintf(page, "%llu\n",
		(unsigned long long)q->limits.max_write_zeroes_sectors << 9);
}