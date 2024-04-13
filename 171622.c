int ttm_page_alloc_debugfs(struct seq_file *m, void *data)
{
	struct ttm_page_pool *p;
	unsigned i;
	char *h[] = {"pool", "refills", "pages freed", "size"};
	if (!_manager) {
		seq_printf(m, "No pool allocator running.\n");
		return 0;
	}
	seq_printf(m, "%7s %12s %13s %8s\n",
			h[0], h[1], h[2], h[3]);
	for (i = 0; i < NUM_POOLS; ++i) {
		p = &_manager->pools[i];

		seq_printf(m, "%7s %12ld %13ld %8d\n",
				p->name, p->nrefills,
				p->nfrees, p->npages);
	}
	return 0;
}