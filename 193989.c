static ssize_t queue_fua_show(struct request_queue *q, char *page)
{
	return sprintf(page, "%u\n", test_bit(QUEUE_FLAG_FUA, &q->queue_flags));
}