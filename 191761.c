void netlink_detachskb(struct sock *sk, struct sk_buff *skb)
{
	kfree_skb(skb);
	sock_put(sk);
}