xfrm_policy_inexact_insert_use_any_list(const struct xfrm_policy *policy)
{
	const xfrm_address_t *addr;
	bool saddr_any, daddr_any;
	u8 prefixlen;

	addr = &policy->selector.saddr;
	prefixlen = policy->selector.prefixlen_s;

	saddr_any = xfrm_pol_inexact_addr_use_any_list(addr,
						       policy->family,
						       prefixlen);
	addr = &policy->selector.daddr;
	prefixlen = policy->selector.prefixlen_d;
	daddr_any = xfrm_pol_inexact_addr_use_any_list(addr,
						       policy->family,
						       prefixlen);
	return saddr_any && daddr_any;
}