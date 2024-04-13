rdpsnd_queue_next_tick(void)
{
	if (((queue_lo + 1) % MAX_QUEUE) != queue_hi)
	{
		return packet_queue[(queue_lo + 1) % MAX_QUEUE].tick;
	}
	else
	{
		return (packet_queue[queue_lo].tick + 65535) % 65536;
	}
}