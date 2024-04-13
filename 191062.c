void dccp_send_sync(struct sock *sk, const u64 ackno,
		    const enum dccp_pkt_type pkt_type)
{
	/*
	 * We are not putting this on the write queue, so
	 * dccp_transmit_skb() will set the ownership to this
	 * sock.
	 */
	struct sk_buff *skb = alloc_skb(sk->sk_prot->max_header, GFP_ATOMIC);

	if (skb == NULL) {
		/* FIXME: how to make sure the sync is sent? */
		DCCP_CRIT("could not send %s", dccp_packet_name(pkt_type));
		return;
	}

	/* Reserve space for headers and prepare control bits. */
	skb_reserve(skb, sk->sk_prot->max_header);
	DCCP_SKB_CB(skb)->dccpd_type = pkt_type;
	DCCP_SKB_CB(skb)->dccpd_ack_seq = ackno;

	dccp_transmit_skb(sk, skb);
}