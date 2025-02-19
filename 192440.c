static inline size_t flow_keys_hash_length(struct flow_keys *flow)
{
	size_t diff = FLOW_KEYS_HASH_OFFSET + sizeof(flow->addrs);
	BUILD_BUG_ON((sizeof(*flow) - FLOW_KEYS_HASH_OFFSET) % sizeof(u32));
	BUILD_BUG_ON(offsetof(typeof(*flow), addrs) !=
		     sizeof(*flow) - sizeof(flow->addrs));

	switch (flow->control.addr_type) {
	case FLOW_DISSECTOR_KEY_IPV4_ADDRS:
		diff -= sizeof(flow->addrs.v4addrs);
		break;
	case FLOW_DISSECTOR_KEY_IPV6_ADDRS:
		diff -= sizeof(flow->addrs.v6addrs);
		break;
	case FLOW_DISSECTOR_KEY_TIPC_ADDRS:
		diff -= sizeof(flow->addrs.tipcaddrs);
		break;
	}
	return (sizeof(*flow) - diff) / sizeof(u32);
}