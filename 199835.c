static int inet6_dump_ifaddr(struct sk_buff *skb, struct netlink_callback *cb)
{
	enum addr_type_t type = UNICAST_ADDR;

	return inet6_dump_addr(skb, cb, type);
}