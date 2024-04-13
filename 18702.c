xfrm_pol_inexact_node_alloc(const xfrm_address_t *addr, u8 prefixlen)
{
	struct xfrm_pol_inexact_node *node;

	node = kzalloc(sizeof(*node), GFP_ATOMIC);
	if (node)
		xfrm_pol_inexact_node_init(node, addr, prefixlen);

	return node;
}