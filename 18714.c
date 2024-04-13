static void xfrm_pol_inexact_node_init(struct xfrm_pol_inexact_node *node,
				       const xfrm_address_t *addr, u8 prefixlen)
{
	node->addr = *addr;
	node->prefixlen = prefixlen;
}