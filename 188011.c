static inline void packet_increment_rx_head(struct packet_sock *po,
					    struct packet_ring_buffer *rb)
{
	switch (po->tp_version) {
	case TPACKET_V1:
	case TPACKET_V2:
		return packet_increment_head(rb);
	case TPACKET_V3:
	default:
		WARN(1, "TPACKET version not supported.\n");
		BUG();
		return;
	}
}