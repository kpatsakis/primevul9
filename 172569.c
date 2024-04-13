static int scrub_write_page_to_dev_replace(struct scrub_block *sblock,
					   int page_num)
{
	struct scrub_page *spage = sblock->pagev[page_num];

	BUG_ON(spage->page == NULL);
	if (spage->io_error) {
		void *mapped_buffer = kmap_atomic(spage->page);

		clear_page(mapped_buffer);
		flush_dcache_page(spage->page);
		kunmap_atomic(mapped_buffer);
	}
	return scrub_add_page_to_wr_bio(sblock->sctx, spage);
}