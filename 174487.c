rdpsnd_queue_complete_pending(void)
{
	struct timeval now;
	long elapsed;
	struct audio_packet *packet;

	gettimeofday(&now, NULL);

	while (queue_pending != queue_lo)
	{
		packet = &packet_queue[queue_pending];

		if (now.tv_sec < packet->completion_tv.tv_sec)
			break;

		if ((now.tv_sec == packet->completion_tv.tv_sec) &&
		    (now.tv_usec < packet->completion_tv.tv_usec))
			break;

		elapsed = (packet->completion_tv.tv_sec - packet->arrive_tv.tv_sec) * 1000000 +
			(packet->completion_tv.tv_usec - packet->arrive_tv.tv_usec);
		elapsed /= 1000;

		xfree(packet->s.data);
		rdpsnd_send_completion((packet->tick + elapsed) % 65536, packet->index);
		queue_pending = (queue_pending + 1) % MAX_QUEUE;
	}
}