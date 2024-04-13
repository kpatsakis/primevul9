static bool xfrm_pol_inexact_addr_use_any_list(const xfrm_address_t *addr,
					       int family, u8 prefixlen)
{
	if (xfrm_addr_any(addr, family))
		return true;

	if (family == AF_INET6 && prefixlen < INEXACT_PREFIXLEN_IPV6)
		return true;

	if (family == AF_INET && prefixlen < INEXACT_PREFIXLEN_IPV4)
		return true;

	return false;
}