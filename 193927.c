queue_ra_store(struct request_queue *q, const char *page, size_t count)
{
	unsigned long ra_kb;
	ssize_t ret = queue_var_store(&ra_kb, page, count);

	if (ret < 0)
		return ret;

	q->backing_dev_info->ra_pages = ra_kb >> (PAGE_SHIFT - 10);

	return ret;
}