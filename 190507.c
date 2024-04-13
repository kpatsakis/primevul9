static int f2fs_submit_page_read(struct inode *inode, struct page *page,
							block_t blkaddr)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	struct bio *bio;

	bio = f2fs_grab_read_bio(inode, blkaddr, 1, 0);
	if (IS_ERR(bio))
		return PTR_ERR(bio);

	/* wait for GCed page writeback via META_MAPPING */
	f2fs_wait_on_block_writeback(inode, blkaddr);

	if (bio_add_page(bio, page, PAGE_SIZE, 0) < PAGE_SIZE) {
		bio_put(bio);
		return -EFAULT;
	}
	ClearPageError(page);
	inc_page_count(sbi, F2FS_RD_DATA);
	__submit_bio(sbi, bio, DATA);
	return 0;
}