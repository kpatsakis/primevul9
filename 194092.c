void blk_queue_flag_set(unsigned int flag, struct request_queue *q)
{
	set_bit(flag, &q->queue_flags);
}