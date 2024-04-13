static ssize_t queue_wc_show(struct request_queue *q, char *page)
{
	if (test_bit(QUEUE_FLAG_WC, &q->queue_flags))
		return sprintf(page, "write back\n");

	return sprintf(page, "write through\n");
}