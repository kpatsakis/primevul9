static ssize_t queue_wb_lat_show(struct request_queue *q, char *page)
{
	if (!wbt_rq_qos(q))
		return -EINVAL;

	return sprintf(page, "%llu\n", div_u64(wbt_get_min_lat(q), 1000));
}