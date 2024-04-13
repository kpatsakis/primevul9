static int xfrm_policy_addr_delta(const xfrm_address_t *a,
				  const xfrm_address_t *b,
				  u8 prefixlen, u16 family)
{
	u32 ma, mb, mask;
	unsigned int pdw, pbi;
	int delta = 0;

	switch (family) {
	case AF_INET:
		if (prefixlen == 0)
			return 0;
		mask = ~0U << (32 - prefixlen);
		ma = ntohl(a->a4) & mask;
		mb = ntohl(b->a4) & mask;
		if (ma < mb)
			delta = -1;
		else if (ma > mb)
			delta = 1;
		break;
	case AF_INET6:
		pdw = prefixlen >> 5;
		pbi = prefixlen & 0x1f;

		if (pdw) {
			delta = memcmp(a->a6, b->a6, pdw << 2);
			if (delta)
				return delta;
		}
		if (pbi) {
			mask = ~0U << (32 - pbi);
			ma = ntohl(a->a6[pdw]) & mask;
			mb = ntohl(b->a6[pdw]) & mask;
			if (ma < mb)
				delta = -1;
			else if (ma > mb)
				delta = 1;
		}
		break;
	default:
		break;
	}

	return delta;
}