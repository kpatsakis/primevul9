int elevator_switch_mq(struct request_queue *q,
			      struct elevator_type *new_e)
{
	int ret;

	lockdep_assert_held(&q->sysfs_lock);

	if (q->elevator) {
		if (q->elevator->registered)
			elv_unregister_queue(q);
		ioc_clear_queue(q);
		elevator_exit(q, q->elevator);
	}

	ret = blk_mq_init_sched(q, new_e);
	if (ret)
		goto out;

	if (new_e) {
		ret = elv_register_queue(q);
		if (ret) {
			elevator_exit(q, q->elevator);
			goto out;
		}
	}

	if (new_e)
		blk_add_trace_msg(q, "elv switch: %s", new_e->elevator_name);
	else
		blk_add_trace_msg(q, "elv switch: none");

out:
	return ret;
}