u64 mdesc_node_by_name(struct mdesc_handle *hp,
		       u64 from_node, const char *name)
{
	struct mdesc_elem *ep = node_block(&hp->mdesc);
	const char *names = name_block(&hp->mdesc);
	u64 last_node = hp->mdesc.node_sz / 16;
	u64 ret;

	if (from_node == MDESC_NODE_NULL) {
		ret = from_node = 0;
	} else if (from_node >= last_node) {
		return MDESC_NODE_NULL;
	} else {
		ret = ep[from_node].d.val;
	}

	while (ret < last_node) {
		if (ep[ret].tag != MD_NODE)
			return MDESC_NODE_NULL;
		if (!strcmp(names + ep[ret].name_offset, name))
			break;
		ret = ep[ret].d.val;
	}
	if (ret >= last_node)
		ret = MDESC_NODE_NULL;
	return ret;
}