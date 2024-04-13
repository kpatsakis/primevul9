static ssize_t queue_poll_show(struct request_queue *q, char *page)
{
	return queue_var_show(test_bit(QUEUE_FLAG_POLL, &q->queue_flags), page);
}