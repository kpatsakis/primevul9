static int xfrm_get_tos(const struct flowi *fl, int family)
{
	if (family == AF_INET)
		return IPTOS_RT_MASK & fl->u.ip4.flowi4_tos;

	return 0;
}