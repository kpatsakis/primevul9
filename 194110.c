void blk_sync_queue(struct request_queue *q)
{
	del_timer_sync(&q->timeout);
	cancel_work_sync(&q->timeout_work);
}