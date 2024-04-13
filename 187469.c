static void *name_block(struct mdesc_hdr *mdesc)
{
	return ((void *) node_block(mdesc)) + mdesc->node_sz;
}