int netlink_sendskb(struct sock *sk, struct sk_buff *skb)
{
	int len = __netlink_sendskb(sk, skb);

	sock_put(sk);
	return len;
}