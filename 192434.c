__be32 flow_get_u32_dst(const struct flow_keys *flow)
{
	switch (flow->control.addr_type) {
	case FLOW_DISSECTOR_KEY_IPV4_ADDRS:
		return flow->addrs.v4addrs.dst;
	case FLOW_DISSECTOR_KEY_IPV6_ADDRS:
		return (__force __be32)ipv6_addr_hash(
			&flow->addrs.v6addrs.dst);
	default:
		return 0;
	}
}