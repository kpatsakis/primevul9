static inline void __flow_hash_consistentify(struct flow_keys *keys)
{
	int addr_diff, i;

	switch (keys->control.addr_type) {
	case FLOW_DISSECTOR_KEY_IPV4_ADDRS:
		addr_diff = (__force u32)keys->addrs.v4addrs.dst -
			    (__force u32)keys->addrs.v4addrs.src;
		if ((addr_diff < 0) ||
		    (addr_diff == 0 &&
		     ((__force u16)keys->ports.dst <
		      (__force u16)keys->ports.src))) {
			swap(keys->addrs.v4addrs.src, keys->addrs.v4addrs.dst);
			swap(keys->ports.src, keys->ports.dst);
		}
		break;
	case FLOW_DISSECTOR_KEY_IPV6_ADDRS:
		addr_diff = memcmp(&keys->addrs.v6addrs.dst,
				   &keys->addrs.v6addrs.src,
				   sizeof(keys->addrs.v6addrs.dst));
		if ((addr_diff < 0) ||
		    (addr_diff == 0 &&
		     ((__force u16)keys->ports.dst <
		      (__force u16)keys->ports.src))) {
			for (i = 0; i < 4; i++)
				swap(keys->addrs.v6addrs.src.s6_addr32[i],
				     keys->addrs.v6addrs.dst.s6_addr32[i]);
			swap(keys->ports.src, keys->ports.dst);
		}
		break;
	}
}