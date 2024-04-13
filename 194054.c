queue_rq_affinity_store(struct request_queue *q, const char *page, size_t count)
{
	ssize_t ret = -EINVAL;
#ifdef CONFIG_SMP
	unsigned long val;

	ret = queue_var_store(&val, page, count);
	if (ret < 0)
		return ret;

	if (val == 2) {
		blk_queue_flag_set(QUEUE_FLAG_SAME_COMP, q);
		blk_queue_flag_set(QUEUE_FLAG_SAME_FORCE, q);
	} else if (val == 1) {
		blk_queue_flag_set(QUEUE_FLAG_SAME_COMP, q);
		blk_queue_flag_clear(QUEUE_FLAG_SAME_FORCE, q);
	} else if (val == 0) {
		blk_queue_flag_clear(QUEUE_FLAG_SAME_COMP, q);
		blk_queue_flag_clear(QUEUE_FLAG_SAME_FORCE, q);
	}
#endif
	return ret;
}