static int __ext4_block_zero_page_range(handle_t *handle,
		struct address_space *mapping, loff_t from, loff_t length)
{
	ext4_fsblk_t index = from >> PAGE_SHIFT;
	unsigned offset = from & (PAGE_SIZE-1);
	unsigned blocksize, pos;
	ext4_lblk_t iblock;
	struct inode *inode = mapping->host;
	struct buffer_head *bh;
	struct page *page;
	int err = 0;

	page = find_or_create_page(mapping, from >> PAGE_SHIFT,
				   mapping_gfp_constraint(mapping, ~__GFP_FS));
	if (!page)
		return -ENOMEM;

	blocksize = inode->i_sb->s_blocksize;

	iblock = index << (PAGE_SHIFT - inode->i_sb->s_blocksize_bits);

	if (!page_has_buffers(page))
		create_empty_buffers(page, blocksize, 0);

	/* Find the buffer that contains "offset" */
	bh = page_buffers(page);
	pos = blocksize;
	while (offset >= pos) {
		bh = bh->b_this_page;
		iblock++;
		pos += blocksize;
	}
	if (buffer_freed(bh)) {
		BUFFER_TRACE(bh, "freed: skip");
		goto unlock;
	}
	if (!buffer_mapped(bh)) {
		BUFFER_TRACE(bh, "unmapped");
		ext4_get_block(inode, iblock, bh, 0);
		/* unmapped? It's a hole - nothing to do */
		if (!buffer_mapped(bh)) {
			BUFFER_TRACE(bh, "still unmapped");
			goto unlock;
		}
	}

	/* Ok, it's mapped. Make sure it's up-to-date */
	if (PageUptodate(page))
		set_buffer_uptodate(bh);

	if (!buffer_uptodate(bh)) {
		err = -EIO;
		ll_rw_block(REQ_OP_READ, 0, 1, &bh);
		wait_on_buffer(bh);
		/* Uhhuh. Read error. Complain and punt. */
		if (!buffer_uptodate(bh))
			goto unlock;
		if (S_ISREG(inode->i_mode) && IS_ENCRYPTED(inode)) {
			/* We expect the key to be set. */
			BUG_ON(!fscrypt_has_encryption_key(inode));
			WARN_ON_ONCE(fscrypt_decrypt_pagecache_blocks(
					page, blocksize, bh_offset(bh)));
		}
	}
	if (ext4_should_journal_data(inode)) {
		BUFFER_TRACE(bh, "get write access");
		err = ext4_journal_get_write_access(handle, bh);
		if (err)
			goto unlock;
	}
	zero_user(page, offset, length);
	BUFFER_TRACE(bh, "zeroed end of block");

	if (ext4_should_journal_data(inode)) {
		err = ext4_handle_dirty_metadata(handle, inode, bh);
	} else {
		err = 0;
		mark_buffer_dirty(bh);
		if (ext4_should_order_data(inode))
			err = ext4_jbd2_inode_add_write(handle, inode, from,
					length);
	}

unlock:
	unlock_page(page);
	put_page(page);
	return err;
}