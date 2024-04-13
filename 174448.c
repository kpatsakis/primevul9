rdpsnd_queue_next_completion(void)
{
	struct audio_packet *packet;
	long remaining;
	struct timeval now;

	if (queue_pending == queue_lo)
		return -1;

	gettimeofday(&now, NULL);

	packet = &packet_queue[queue_pending];

	remaining = (packet->completion_tv.tv_sec - now.tv_sec) * 1000000 +
		(packet->completion_tv.tv_usec - now.tv_usec);

	if (remaining < 0)
		return 0;

	return remaining;
}