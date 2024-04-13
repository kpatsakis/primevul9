int dccp_retransmit_skb(struct sock *sk, struct sk_buff *skb)
{
	if (inet_csk(sk)->icsk_af_ops->rebuild_header(sk) != 0)
		return -EHOSTUNREACH; /* Routing failure or similar. */

	return dccp_transmit_skb(sk, (skb_cloned(skb) ?
				      pskb_copy(skb, GFP_ATOMIC):
				      skb_clone(skb, GFP_ATOMIC)));
}