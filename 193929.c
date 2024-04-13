static void blk_release_queue(struct kobject *kobj)
{
	struct request_queue *q =
		container_of(kobj, struct request_queue, kobj);

	INIT_WORK(&q->release_work, __blk_release_queue);
	schedule_work(&q->release_work);
}