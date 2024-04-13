static inline unsigned int bio_allowed_max_sectors(struct request_queue *q)
{
	return round_down(UINT_MAX, queue_logical_block_size(q)) >> 9;
}