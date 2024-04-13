static struct in6_addr *extract_addr(struct nlattr *addr, struct nlattr *local,
				     struct in6_addr **peer_pfx)
{
	struct in6_addr *pfx = NULL;

	*peer_pfx = NULL;

	if (addr)
		pfx = nla_data(addr);

	if (local) {
		if (pfx && nla_memcmp(local, pfx, sizeof(*pfx)))
			*peer_pfx = pfx;
		pfx = nla_data(local);
	}

	return pfx;
}