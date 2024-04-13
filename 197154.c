static u32 bond_ip_hash(u32 hash, struct flow_keys *flow)
{
	hash ^= (__force u32)flow_get_u32_dst(flow) ^
		(__force u32)flow_get_u32_src(flow);
	hash ^= (hash >> 16);
	hash ^= (hash >> 8);
	/* discard lowest hash bit to deal with the common even ports pattern */
	return hash >> 1;
}