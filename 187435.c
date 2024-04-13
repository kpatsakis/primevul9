const char *mdesc_node_name(struct mdesc_handle *hp, u64 node)
{
	struct mdesc_elem *ep, *base = node_block(&hp->mdesc);
	const char *names = name_block(&hp->mdesc);
	u64 last_node = hp->mdesc.node_sz / 16;

	if (node == MDESC_NODE_NULL || node >= last_node)
		return NULL;

	ep = base + node;
	if (ep->tag != MD_NODE)
		return NULL;

	return names + ep->name_offset;
}