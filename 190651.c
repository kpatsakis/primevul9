int f2fs_do_write_data_page(struct f2fs_io_info *fio)
{
	struct page *page = fio->page;
	struct inode *inode = page->mapping->host;
	struct dnode_of_data dn;
	struct extent_info ei = {0,0,0};
	struct node_info ni;
	bool ipu_force = false;
	int err = 0;

	set_new_dnode(&dn, inode, NULL, NULL, 0);
	if (need_inplace_update(fio) &&
			f2fs_lookup_extent_cache(inode, page->index, &ei)) {
		fio->old_blkaddr = ei.blk + page->index - ei.fofs;

		if (!f2fs_is_valid_blkaddr(fio->sbi, fio->old_blkaddr,
						DATA_GENERIC_ENHANCE))
			return -EFSCORRUPTED;

		ipu_force = true;
		fio->need_lock = LOCK_DONE;
		goto got_it;
	}

	/* Deadlock due to between page->lock and f2fs_lock_op */
	if (fio->need_lock == LOCK_REQ && !f2fs_trylock_op(fio->sbi))
		return -EAGAIN;

	err = f2fs_get_dnode_of_data(&dn, page->index, LOOKUP_NODE);
	if (err)
		goto out;

	fio->old_blkaddr = dn.data_blkaddr;

	/* This page is already truncated */
	if (fio->old_blkaddr == NULL_ADDR) {
		ClearPageUptodate(page);
		clear_cold_data(page);
		goto out_writepage;
	}
got_it:
	if (__is_valid_data_blkaddr(fio->old_blkaddr) &&
		!f2fs_is_valid_blkaddr(fio->sbi, fio->old_blkaddr,
						DATA_GENERIC_ENHANCE)) {
		err = -EFSCORRUPTED;
		goto out_writepage;
	}
	/*
	 * If current allocation needs SSR,
	 * it had better in-place writes for updated data.
	 */
	if (ipu_force ||
		(__is_valid_data_blkaddr(fio->old_blkaddr) &&
					need_inplace_update(fio))) {
		err = encrypt_one_page(fio);
		if (err)
			goto out_writepage;

		set_page_writeback(page);
		ClearPageError(page);
		f2fs_put_dnode(&dn);
		if (fio->need_lock == LOCK_REQ)
			f2fs_unlock_op(fio->sbi);
		err = f2fs_inplace_write_data(fio);
		if (err) {
			if (f2fs_encrypted_file(inode))
				fscrypt_pullback_bio_page(&fio->encrypted_page,
									true);
			if (PageWriteback(page))
				end_page_writeback(page);
		} else {
			set_inode_flag(inode, FI_UPDATE_WRITE);
		}
		trace_f2fs_do_write_data_page(fio->page, IPU);
		return err;
	}

	if (fio->need_lock == LOCK_RETRY) {
		if (!f2fs_trylock_op(fio->sbi)) {
			err = -EAGAIN;
			goto out_writepage;
		}
		fio->need_lock = LOCK_REQ;
	}

	err = f2fs_get_node_info(fio->sbi, dn.nid, &ni);
	if (err)
		goto out_writepage;

	fio->version = ni.version;

	err = encrypt_one_page(fio);
	if (err)
		goto out_writepage;

	set_page_writeback(page);
	ClearPageError(page);

	/* LFS mode write path */
	f2fs_outplace_write_data(&dn, fio);
	trace_f2fs_do_write_data_page(page, OPU);
	set_inode_flag(inode, FI_APPEND_WRITE);
	if (page->index == 0)
		set_inode_flag(inode, FI_FIRST_BLOCK_WRITTEN);
out_writepage:
	f2fs_put_dnode(&dn);
out:
	if (fio->need_lock == LOCK_REQ)
		f2fs_unlock_op(fio->sbi);
	return err;
}