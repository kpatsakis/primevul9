__u32 __get_hash_from_flowi6(const struct flowi6 *fl6, struct flow_keys *keys)
{
	memset(keys, 0, sizeof(*keys));

	memcpy(&keys->addrs.v6addrs.src, &fl6->saddr,
	    sizeof(keys->addrs.v6addrs.src));
	memcpy(&keys->addrs.v6addrs.dst, &fl6->daddr,
	    sizeof(keys->addrs.v6addrs.dst));
	keys->control.addr_type = FLOW_DISSECTOR_KEY_IPV6_ADDRS;
	keys->ports.src = fl6->fl6_sport;
	keys->ports.dst = fl6->fl6_dport;
	keys->keyid.keyid = fl6->fl6_gre_key;
	keys->tags.flow_label = (__force u32)fl6->flowlabel;
	keys->basic.ip_proto = fl6->flowi6_proto;

	return flow_hash_from_keys(keys);
}