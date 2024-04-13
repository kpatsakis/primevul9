bool netlink_capable(const struct sk_buff *skb, int cap)
{
	return netlink_ns_capable(skb, &init_user_ns, cap);
}