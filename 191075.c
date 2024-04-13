static inline void dccp_event_ack_sent(struct sock *sk)
{
	inet_csk_clear_xmit_timer(sk, ICSK_TIME_DACK);
}