static u32 ip_mc_hash(const struct ip_mc_list *im)
{
	return hash_32((__force u32)im->multiaddr, MC_HASH_SZ_LOG);
}