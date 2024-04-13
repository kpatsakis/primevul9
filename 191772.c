bool netlink_net_capable(const struct sk_buff *skb, int cap)
{
	return netlink_ns_capable(skb, sock_net(skb->sk)->user_ns, cap);
}