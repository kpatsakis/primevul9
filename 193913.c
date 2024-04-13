void blk_queue_flag_clear(unsigned int flag, struct request_queue *q)
{
	clear_bit(flag, &q->queue_flags);
}