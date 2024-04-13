void dccp_send_close(struct sock *sk, const int active)
{
	struct dccp_sock *dp = dccp_sk(sk);
	struct sk_buff *skb;
	const gfp_t prio = active ? GFP_KERNEL : GFP_ATOMIC;

	skb = alloc_skb(sk->sk_prot->max_header, prio);
	if (skb == NULL)
		return;

	/* Reserve space for headers and prepare control bits. */
	skb_reserve(skb, sk->sk_prot->max_header);
	DCCP_SKB_CB(skb)->dccpd_type = dp->dccps_role == DCCP_ROLE_CLIENT ?
					DCCP_PKT_CLOSE : DCCP_PKT_CLOSEREQ;

	if (active) {
		dccp_write_xmit(sk, 1);
		dccp_skb_entail(sk, skb);
		dccp_transmit_skb(sk, skb_clone(skb, prio));
		/* FIXME do we need a retransmit timer here? */
	} else
		dccp_transmit_skb(sk, skb);
}