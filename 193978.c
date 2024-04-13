static inline void elevator_exit(struct request_queue *q,
		struct elevator_queue *e)
{
	blk_mq_sched_free_requests(q);
	__elevator_exit(q, e);
}