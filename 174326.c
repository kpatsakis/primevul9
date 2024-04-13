rdpsnd_queue_next(unsigned long completed_in_us)
{
	struct audio_packet *packet;

	assert(!rdpsnd_queue_empty());

	packet = &packet_queue[queue_lo];

	gettimeofday(&packet->completion_tv, NULL);

	packet->completion_tv.tv_usec += completed_in_us;
	packet->completion_tv.tv_sec += packet->completion_tv.tv_usec / 1000000;
	packet->completion_tv.tv_usec %= 1000000;

	queue_lo = (queue_lo + 1) % MAX_QUEUE;

	rdpsnd_queue_complete_pending();
}