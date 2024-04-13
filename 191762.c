bool netlink_ns_capable(const struct sk_buff *skb,
			struct user_namespace *user_ns, int cap)
{
	return __netlink_ns_capable(&NETLINK_CB(skb), user_ns, cap);
}