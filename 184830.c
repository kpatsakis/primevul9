static u8 ip4_frag_ecn(u8 tos)
{
	return 1 << (tos & INET_ECN_MASK);
}