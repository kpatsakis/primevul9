u64 mdesc_arc_target(struct mdesc_handle *hp, u64 arc)
{
	struct mdesc_elem *ep, *base = node_block(&hp->mdesc);

	ep = base + arc;

	return ep->d.val;
}