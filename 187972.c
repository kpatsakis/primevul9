static inline void *packet_previous_frame(struct packet_sock *po,
		struct packet_ring_buffer *rb,
		int status)
{
	unsigned int previous = rb->head ? rb->head - 1 : rb->frame_max;
	return packet_lookup_frame(po, rb, previous, status);
}