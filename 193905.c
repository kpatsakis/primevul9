bool blk_queue_flag_test_and_set(unsigned int flag, struct request_queue *q)
{
	return test_and_set_bit(flag, &q->queue_flags);
}