rdpsnd_queue_clear(void)
{
	struct audio_packet *packet;

	/* Go through everything, not just the pending packets */
	while (queue_pending != queue_hi)
	{
		packet = &packet_queue[queue_pending];
		xfree(packet->s.data);
		queue_pending = (queue_pending + 1) % MAX_QUEUE;
	}

	/* Reset everything back to the initial state */
	queue_pending = queue_lo = queue_hi = 0;
}