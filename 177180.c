static int ext4_xattr_ensure_credits(handle_t *handle, struct inode *inode,
				     int credits, struct buffer_head *bh,
				     bool dirty, bool block_csum)
{
	int error;

	if (!ext4_handle_valid(handle))
		return 0;

	if (handle->h_buffer_credits >= credits)
		return 0;

	error = ext4_journal_extend(handle, credits - handle->h_buffer_credits);
	if (!error)
		return 0;
	if (error < 0) {
		ext4_warning(inode->i_sb, "Extend journal (error %d)", error);
		return error;
	}

	if (bh && dirty) {
		if (block_csum)
			ext4_xattr_block_csum_set(inode, bh);
		error = ext4_handle_dirty_metadata(handle, NULL, bh);
		if (error) {
			ext4_warning(inode->i_sb, "Handle metadata (error %d)",
				     error);
			return error;
		}
	}

	error = ext4_journal_restart(handle, credits);
	if (error) {
		ext4_warning(inode->i_sb, "Restart journal (error %d)", error);
		return error;
	}

	if (bh) {
		error = ext4_journal_get_write_access(handle, bh);
		if (error) {
			ext4_warning(inode->i_sb,
				     "Get write access failed (error %d)",
				     error);
			return error;
		}
	}
	return 0;
}