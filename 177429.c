static struct page *new_page(struct page *p, unsigned long private)
{
	int nid = page_to_nid(p);

	return new_page_nodemask(p, nid, &node_states[N_MEMORY]);
}