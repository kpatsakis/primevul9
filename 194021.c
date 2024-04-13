static ssize_t queue_ra_show(struct request_queue *q, char *page)
{
	unsigned long ra_kb = q->backing_dev_info->ra_pages <<
					(PAGE_SHIFT - 10);

	return queue_var_show(ra_kb, (page));
}