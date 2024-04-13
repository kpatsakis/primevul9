static void collect_procs(struct page *page, struct list_head *tokill,
				int force_early)
{
	struct to_kill *tk;

	if (!page->mapping)
		return;

	tk = kmalloc(sizeof(struct to_kill), GFP_NOIO);
	if (!tk)
		return;
	if (PageAnon(page))
		collect_procs_anon(page, tokill, &tk, force_early);
	else
		collect_procs_file(page, tokill, &tk, force_early);
	kfree(tk);
}