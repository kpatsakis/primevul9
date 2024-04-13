static void f2fs_submit_ipu_bio(struct f2fs_sb_info *sbi, struct bio **bio,
							struct page *page)
{
	if (!bio)
		return;

	if (!__has_merged_page(*bio, NULL, page, 0))
		return;

	__submit_bio(sbi, *bio, DATA);
	*bio = NULL;
}