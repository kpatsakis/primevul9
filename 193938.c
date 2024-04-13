static inline void __blk_get_queue(struct request_queue *q)
{
	kobject_get(&q->kobj);
}