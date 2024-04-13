static ssize_t queue_discard_max_show(struct request_queue *q, char *page)
{
	return sprintf(page, "%llu\n",
		       (unsigned long long)q->limits.max_discard_sectors << 9);
}