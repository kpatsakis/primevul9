static int bdev_try_to_free_page(struct super_block *sb, struct page *page,
				 gfp_t wait)
{
	journal_t *journal = EXT4_SB(sb)->s_journal;

	WARN_ON(PageChecked(page));
	if (!page_has_buffers(page))
		return 0;
	if (journal)
		return jbd2_journal_try_to_free_buffers(journal, page,
							wait & ~__GFP_WAIT);
	return try_to_free_buffers(page);
}