static int elevator_switch(struct request_queue *q, struct elevator_type *new_e)
{
	int err;

	lockdep_assert_held(&q->sysfs_lock);

	blk_mq_freeze_queue(q);
	blk_mq_quiesce_queue(q);

	err = elevator_switch_mq(q, new_e);

	blk_mq_unquiesce_queue(q);
	blk_mq_unfreeze_queue(q);

	return err;
}