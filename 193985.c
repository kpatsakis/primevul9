static ssize_t queue_zoned_show(struct request_queue *q, char *page)
{
	switch (blk_queue_zoned_model(q)) {
	case BLK_ZONED_HA:
		return sprintf(page, "host-aware\n");
	case BLK_ZONED_HM:
		return sprintf(page, "host-managed\n");
	default:
		return sprintf(page, "none\n");
	}
}