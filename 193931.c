void blk_set_pm_only(struct request_queue *q)
{
	atomic_inc(&q->pm_only);
}