void dccp_send_ack(struct sock *sk)
{
	/* If we have been reset, we may not send again. */
	if (sk->sk_state != DCCP_CLOSED) {
		struct sk_buff *skb = alloc_skb(sk->sk_prot->max_header,
						GFP_ATOMIC);

		if (skb == NULL) {
			inet_csk_schedule_ack(sk);
			inet_csk(sk)->icsk_ack.ato = TCP_ATO_MIN;
			inet_csk_reset_xmit_timer(sk, ICSK_TIME_DACK,
						  TCP_DELACK_MAX,
						  DCCP_RTO_MAX);
			return;
		}

		/* Reserve space for headers */
		skb_reserve(skb, sk->sk_prot->max_header);
		DCCP_SKB_CB(skb)->dccpd_type = DCCP_PKT_ACK;
		dccp_transmit_skb(sk, skb);
	}
}