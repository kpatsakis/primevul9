queue_max_sectors_store(struct request_queue *q, const char *page, size_t count)
{
	unsigned long max_sectors_kb,
		max_hw_sectors_kb = queue_max_hw_sectors(q) >> 1,
			page_kb = 1 << (PAGE_SHIFT - 10);
	ssize_t ret = queue_var_store(&max_sectors_kb, page, count);

	if (ret < 0)
		return ret;

	max_hw_sectors_kb = min_not_zero(max_hw_sectors_kb, (unsigned long)
					 q->limits.max_dev_sectors >> 1);

	if (max_sectors_kb > max_hw_sectors_kb || max_sectors_kb < page_kb)
		return -EINVAL;

	spin_lock_irq(&q->queue_lock);
	q->limits.max_sectors = max_sectors_kb << 1;
	q->backing_dev_info->io_pages = max_sectors_kb >> (PAGE_SHIFT - 10);
	spin_unlock_irq(&q->queue_lock);

	return ret;
}