int blk_lld_busy(struct request_queue *q)
{
	if (queue_is_mq(q) && q->mq_ops->busy)
		return q->mq_ops->busy(q);

	return 0;
}