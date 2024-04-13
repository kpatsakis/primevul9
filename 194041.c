queue_requests_store(struct request_queue *q, const char *page, size_t count)
{
	unsigned long nr;
	int ret, err;

	if (!queue_is_mq(q))
		return -EINVAL;

	ret = queue_var_store(&nr, page, count);
	if (ret < 0)
		return ret;

	if (nr < BLKDEV_MIN_RQ)
		nr = BLKDEV_MIN_RQ;

	err = blk_mq_update_nr_requests(q, nr);
	if (err)
		return err;

	return ret;
}