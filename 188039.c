static inline void *packet_previous_rx_frame(struct packet_sock *po,
					     struct packet_ring_buffer *rb,
					     int status)
{
	if (po->tp_version <= TPACKET_V2)
		return packet_previous_frame(po, rb, status);

	return __prb_previous_block(po, rb, status);
}