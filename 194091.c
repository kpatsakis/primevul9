ssize_t elv_iosched_store(struct request_queue *q, const char *name,
			  size_t count)
{
	int ret;

	if (!queue_is_mq(q) || !elv_support_iosched(q))
		return count;

	ret = __elevator_change(q, name);
	if (!ret)
		return count;

	return ret;
}