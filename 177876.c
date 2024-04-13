mISDN_sock_cmsg(struct sock *sk, struct msghdr *msg, struct sk_buff *skb)
{
	struct __kernel_old_timeval	tv;

	if (_pms(sk)->cmask & MISDN_TIME_STAMP) {
		skb_get_timestamp(skb, &tv);
		put_cmsg(msg, SOL_MISDN, MISDN_TIME_STAMP, sizeof(tv), &tv);
	}
}