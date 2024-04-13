__u32 __skb_get_hash_flowi4(struct sk_buff *skb, const struct flowi4 *fl4)
{
	struct flow_keys keys;

	memset(&keys, 0, sizeof(keys));

	keys.addrs.v4addrs.src = fl4->saddr;
	keys.addrs.v4addrs.dst = fl4->daddr;
	keys.control.addr_type = FLOW_DISSECTOR_KEY_IPV4_ADDRS;
	keys.ports.src = fl4->fl4_sport;
	keys.ports.dst = fl4->fl4_dport;
	keys.keyid.keyid = fl4->fl4_gre_key;
	keys.basic.ip_proto = fl4->flowi4_proto;

	__skb_set_sw_hash(skb, flow_hash_from_keys(&keys),
			  flow_keys_have_l4(&keys));

	return skb->hash;
}