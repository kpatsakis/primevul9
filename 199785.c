static int inet6_dump_ifmcaddr(struct sk_buff *skb, struct netlink_callback *cb)
{
	enum addr_type_t type = MULTICAST_ADDR;

	return inet6_dump_addr(skb, cb, type);
}