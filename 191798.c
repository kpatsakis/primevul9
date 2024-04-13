static int __netlink_sendskb(struct sock *sk, struct sk_buff *skb)
{
	int len = skb->len;

	netlink_deliver_tap(skb);

	skb_queue_tail(&sk->sk_receive_queue, skb);
	sk->sk_data_ready(sk);
	return len;
}