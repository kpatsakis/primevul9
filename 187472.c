u64 mdesc_next_arc(struct mdesc_handle *hp, u64 from, const char *arc_type)
{
	struct mdesc_elem *ep, *base = node_block(&hp->mdesc);
	const char *names = name_block(&hp->mdesc);
	u64 last_node = hp->mdesc.node_sz / 16;

	if (from == MDESC_NODE_NULL || from >= last_node)
		return MDESC_NODE_NULL;

	ep = base + from;

	ep++;
	for (; ep->tag != MD_NODE_END; ep++) {
		if (ep->tag != MD_PROP_ARC)
			continue;

		if (strcmp(names + ep->name_offset, arc_type))
			continue;

		return ep - base;
	}

	return MDESC_NODE_NULL;
}