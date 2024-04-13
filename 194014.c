void blk_set_queue_dying(struct request_queue *q)
{
	blk_queue_flag_set(QUEUE_FLAG_DYING, q);

	/*
	 * When queue DYING flag is set, we need to block new req
	 * entering queue, so we call blk_freeze_queue_start() to
	 * prevent I/O from crossing blk_queue_enter().
	 */
	blk_freeze_queue_start(q);

	if (queue_is_mq(q))
		blk_mq_wake_waiters(q);

	/* Make blk_queue_enter() reexamine the DYING flag. */
	wake_up_all(&q->mq_freeze_wq);
}