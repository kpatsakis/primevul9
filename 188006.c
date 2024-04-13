static inline void *packet_current_rx_frame(struct packet_sock *po,
					    struct sk_buff *skb,
					    int status, unsigned int len)
{
	char *curr = NULL;
	switch (po->tp_version) {
	case TPACKET_V1:
	case TPACKET_V2:
		curr = packet_lookup_frame(po, &po->rx_ring,
					po->rx_ring.head, status);
		return curr;
	case TPACKET_V3:
		return __packet_lookup_frame_in_block(po, skb, status, len);
	default:
		WARN(1, "TPACKET version not supported\n");
		BUG();
		return 0;
	}
}