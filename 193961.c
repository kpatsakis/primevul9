static ssize_t queue_poll_delay_show(struct request_queue *q, char *page)
{
	int val;

	if (q->poll_nsec == BLK_MQ_POLL_CLASSIC)
		val = BLK_MQ_POLL_CLASSIC;
	else
		val = q->poll_nsec / 1000;

	return sprintf(page, "%d\n", val);
}