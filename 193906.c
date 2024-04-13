void blk_put_queue(struct request_queue *q)
{
	kobject_put(&q->kobj);
}