int dccp_connect(struct sock *sk)
{
	struct sk_buff *skb;
	struct inet_connection_sock *icsk = inet_csk(sk);

	dccp_connect_init(sk);

	skb = alloc_skb(sk->sk_prot->max_header, sk->sk_allocation);
	if (unlikely(skb == NULL))
		return -ENOBUFS;

	/* Reserve space for headers. */
	skb_reserve(skb, sk->sk_prot->max_header);

	DCCP_SKB_CB(skb)->dccpd_type = DCCP_PKT_REQUEST;

	dccp_skb_entail(sk, skb);
	dccp_transmit_skb(sk, skb_clone(skb, GFP_KERNEL));
	DCCP_INC_STATS(DCCP_MIB_ACTIVEOPENS);

	/* Timer for repeating the REQUEST until an answer. */
	inet_csk_reset_xmit_timer(sk, ICSK_TIME_RETRANS,
				  icsk->icsk_rto, DCCP_RTO_MAX);
	return 0;
}