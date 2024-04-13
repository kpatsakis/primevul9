static void __submit_merged_write_cond(struct f2fs_sb_info *sbi,
				struct inode *inode, struct page *page,
				nid_t ino, enum page_type type, bool force)
{
	enum temp_type temp;
	bool ret = true;

	for (temp = HOT; temp < NR_TEMP_TYPE; temp++) {
		if (!force)	{
			enum page_type btype = PAGE_TYPE_OF_BIO(type);
			struct f2fs_bio_info *io = sbi->write_io[btype] + temp;

			down_read(&io->io_rwsem);
			ret = __has_merged_page(io->bio, inode, page, ino);
			up_read(&io->io_rwsem);
		}
		if (ret)
			__f2fs_submit_merged_write(sbi, type, temp);

		/* TODO: use HOT temp only for meta pages now. */
		if (type >= META)
			break;
	}
}