static ssize_t queue_wb_lat_store(struct request_queue *q, const char *page,
				  size_t count)
{
	struct rq_qos *rqos;
	ssize_t ret;
	s64 val;

	ret = queue_var_store64(&val, page);
	if (ret < 0)
		return ret;
	if (val < -1)
		return -EINVAL;

	rqos = wbt_rq_qos(q);
	if (!rqos) {
		ret = wbt_init(q);
		if (ret)
			return ret;
	}

	if (val == -1)
		val = wbt_default_latency_nsec(q);
	else if (val >= 0)
		val *= 1000ULL;

	if (wbt_get_min_lat(q) == val)
		return count;

	/*
	 * Ensure that the queue is idled, in case the latency update
	 * ends up either enabling or disabling wbt completely. We can't
	 * have IO inflight if that happens.
	 */
	blk_mq_freeze_queue(q);
	blk_mq_quiesce_queue(q);

	wbt_set_min_lat(q, val);
	wbt_update_limits(q);

	blk_mq_unquiesce_queue(q);
	blk_mq_unfreeze_queue(q);

	return count;
}