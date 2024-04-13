static int f2fs_drop_inode(struct inode *inode)
{
	/*
	 * This is to avoid a deadlock condition like below.
	 * writeback_single_inode(inode)
	 *  - f2fs_write_data_page
	 *    - f2fs_gc -> iput -> evict
	 *       - inode_wait_for_writeback(inode)
	 */
	if (!inode_unhashed(inode) && inode->i_state & I_SYNC) {
		if (!inode->i_nlink && !is_bad_inode(inode)) {
			/* to avoid evict_inode call simultaneously */
			atomic_inc(&inode->i_count);
			spin_unlock(&inode->i_lock);

			/* some remained atomic pages should discarded */
			if (f2fs_is_atomic_file(inode))
				commit_inmem_pages(inode, true);

			/* should remain fi->extent_tree for writepage */
			f2fs_destroy_extent_node(inode);

			sb_start_intwrite(inode->i_sb);
			i_size_write(inode, 0);

			if (F2FS_HAS_BLOCKS(inode))
				f2fs_truncate(inode, true);

			sb_end_intwrite(inode->i_sb);

#ifdef CONFIG_F2FS_FS_ENCRYPTION
			if (F2FS_I(inode)->i_crypt_info)
				f2fs_free_encryption_info(inode,
					F2FS_I(inode)->i_crypt_info);
#endif
			spin_lock(&inode->i_lock);
			atomic_dec(&inode->i_count);
		}
		return 0;
	}
	return generic_drop_inode(inode);
}