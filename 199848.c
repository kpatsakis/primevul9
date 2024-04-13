static u32 inet6_addr_hash(const struct in6_addr *addr)
{
	return hash_32(ipv6_addr_hash(addr), IN6_ADDR_HSIZE_SHIFT);
}