static struct mdesc_elem *node_block(struct mdesc_hdr *mdesc)
{
	return (struct mdesc_elem *) (mdesc + 1);
}