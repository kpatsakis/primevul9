rdpsnd_queue_empty(void)
{
	return (queue_lo == queue_hi);
}