static int encrypt_one_page(struct f2fs_io_info *fio)
{
	struct inode *inode = fio->page->mapping->host;
	struct page *mpage;
	gfp_t gfp_flags = GFP_NOFS;

	if (!f2fs_encrypted_file(inode))
		return 0;

	/* wait for GCed page writeback via META_MAPPING */
	f2fs_wait_on_block_writeback(inode, fio->old_blkaddr);

retry_encrypt:
	fio->encrypted_page = fscrypt_encrypt_page(inode, fio->page,
			PAGE_SIZE, 0, fio->page->index, gfp_flags);
	if (IS_ERR(fio->encrypted_page)) {
		/* flush pending IOs and wait for a while in the ENOMEM case */
		if (PTR_ERR(fio->encrypted_page) == -ENOMEM) {
			f2fs_flush_merged_writes(fio->sbi);
			congestion_wait(BLK_RW_ASYNC, HZ/50);
			gfp_flags |= __GFP_NOFAIL;
			goto retry_encrypt;
		}
		return PTR_ERR(fio->encrypted_page);
	}

	mpage = find_lock_page(META_MAPPING(fio->sbi), fio->old_blkaddr);
	if (mpage) {
		if (PageUptodate(mpage))
			memcpy(page_address(mpage),
				page_address(fio->encrypted_page), PAGE_SIZE);
		f2fs_put_page(mpage, 1);
	}
	return 0;
}