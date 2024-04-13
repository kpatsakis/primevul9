static inline u8 ip6_frag_ecn(const struct ipv6hdr *ipv6h)
{
	return 1 << (ipv6_get_dsfield(ipv6h) & INET_ECN_MASK);
}