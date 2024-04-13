static int __write_data_page(struct page *page, bool *submitted,
				struct bio **bio,
				sector_t *last_block,
				struct writeback_control *wbc,
				enum iostat_type io_type)
{
	struct inode *inode = page->mapping->host;
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	loff_t i_size = i_size_read(inode);
	const pgoff_t end_index = ((unsigned long long) i_size)
							>> PAGE_SHIFT;
	loff_t psize = (page->index + 1) << PAGE_SHIFT;
	unsigned offset = 0;
	bool need_balance_fs = false;
	int err = 0;
	struct f2fs_io_info fio = {
		.sbi = sbi,
		.ino = inode->i_ino,
		.type = DATA,
		.op = REQ_OP_WRITE,
		.op_flags = wbc_to_write_flags(wbc),
		.old_blkaddr = NULL_ADDR,
		.page = page,
		.encrypted_page = NULL,
		.submitted = false,
		.need_lock = LOCK_RETRY,
		.io_type = io_type,
		.io_wbc = wbc,
		.bio = bio,
		.last_block = last_block,
	};

	trace_f2fs_writepage(page, DATA);

	/* we should bypass data pages to proceed the kworkder jobs */
	if (unlikely(f2fs_cp_error(sbi))) {
		mapping_set_error(page->mapping, -EIO);
		/*
		 * don't drop any dirty dentry pages for keeping lastest
		 * directory structure.
		 */
		if (S_ISDIR(inode->i_mode))
			goto redirty_out;
		goto out;
	}

	if (unlikely(is_sbi_flag_set(sbi, SBI_POR_DOING)))
		goto redirty_out;

	if (page->index < end_index)
		goto write;

	/*
	 * If the offset is out-of-range of file size,
	 * this page does not have to be written to disk.
	 */
	offset = i_size & (PAGE_SIZE - 1);
	if ((page->index >= end_index + 1) || !offset)
		goto out;

	zero_user_segment(page, offset, PAGE_SIZE);
write:
	if (f2fs_is_drop_cache(inode))
		goto out;
	/* we should not write 0'th page having journal header */
	if (f2fs_is_volatile_file(inode) && (!page->index ||
			(!wbc->for_reclaim &&
			f2fs_available_free_memory(sbi, BASE_CHECK))))
		goto redirty_out;

	/* Dentry blocks are controlled by checkpoint */
	if (S_ISDIR(inode->i_mode)) {
		fio.need_lock = LOCK_DONE;
		err = f2fs_do_write_data_page(&fio);
		goto done;
	}

	if (!wbc->for_reclaim)
		need_balance_fs = true;
	else if (has_not_enough_free_secs(sbi, 0, 0))
		goto redirty_out;
	else
		set_inode_flag(inode, FI_HOT_DATA);

	err = -EAGAIN;
	if (f2fs_has_inline_data(inode)) {
		err = f2fs_write_inline_data(inode, page);
		if (!err)
			goto out;
	}

	if (err == -EAGAIN) {
		err = f2fs_do_write_data_page(&fio);
		if (err == -EAGAIN) {
			fio.need_lock = LOCK_REQ;
			err = f2fs_do_write_data_page(&fio);
		}
	}

	if (err) {
		file_set_keep_isize(inode);
	} else {
		down_write(&F2FS_I(inode)->i_sem);
		if (F2FS_I(inode)->last_disk_size < psize)
			F2FS_I(inode)->last_disk_size = psize;
		up_write(&F2FS_I(inode)->i_sem);
	}

done:
	if (err && err != -ENOENT)
		goto redirty_out;

out:
	inode_dec_dirty_pages(inode);
	if (err) {
		ClearPageUptodate(page);
		clear_cold_data(page);
	}

	if (wbc->for_reclaim) {
		f2fs_submit_merged_write_cond(sbi, NULL, page, 0, DATA);
		clear_inode_flag(inode, FI_HOT_DATA);
		f2fs_remove_dirty_inode(inode);
		submitted = NULL;
	}

	unlock_page(page);
	if (!S_ISDIR(inode->i_mode) && !IS_NOQUOTA(inode) &&
					!F2FS_I(inode)->cp_task) {
		f2fs_submit_ipu_bio(sbi, bio, page);
		f2fs_balance_fs(sbi, need_balance_fs);
	}

	if (unlikely(f2fs_cp_error(sbi))) {
		f2fs_submit_ipu_bio(sbi, bio, page);
		f2fs_submit_merged_write(sbi, DATA);
		submitted = NULL;
	}

	if (submitted)
		*submitted = fio.submitted;

	return 0;

redirty_out:
	redirty_page_for_writepage(wbc, page);
	/*
	 * pageout() in MM traslates EAGAIN, so calls handle_write_error()
	 * -> mapping_set_error() -> set_bit(AS_EIO, ...).
	 * file_write_and_wait_range() will see EIO error, which is critical
	 * to return value of fsync() followed by atomic_write failure to user.
	 */
	if (!err || wbc->for_reclaim)
		return AOP_WRITEPAGE_ACTIVATE;
	unlock_page(page);
	return err;
}