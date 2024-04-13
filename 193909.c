static int __elevator_change(struct request_queue *q, const char *name)
{
	char elevator_name[ELV_NAME_MAX];
	struct elevator_type *e;

	/* Make sure queue is not in the middle of being removed */
	if (!test_bit(QUEUE_FLAG_REGISTERED, &q->queue_flags))
		return -ENOENT;

	/*
	 * Special case for mq, turn off scheduling
	 */
	if (!strncmp(name, "none", 4)) {
		if (!q->elevator)
			return 0;
		return elevator_switch(q, NULL);
	}

	strlcpy(elevator_name, name, sizeof(elevator_name));
	e = elevator_get(q, strstrip(elevator_name), true);
	if (!e)
		return -EINVAL;

	if (q->elevator && elevator_match(q->elevator->type, elevator_name)) {
		elevator_put(e);
		return 0;
	}

	return elevator_switch(q, e);
}