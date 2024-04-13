static int btrfs_remap_file_range_prep(struct file *file_in, loff_t pos_in,
				       struct file *file_out, loff_t pos_out,
				       loff_t *len, unsigned int remap_flags)
{
	struct inode *inode_in = file_inode(file_in);
	struct inode *inode_out = file_inode(file_out);
	u64 bs = BTRFS_I(inode_out)->root->fs_info->sb->s_blocksize;
	bool same_inode = inode_out == inode_in;
	u64 wb_len;
	int ret;

	if (!(remap_flags & REMAP_FILE_DEDUP)) {
		struct btrfs_root *root_out = BTRFS_I(inode_out)->root;

		if (btrfs_root_readonly(root_out))
			return -EROFS;

		if (file_in->f_path.mnt != file_out->f_path.mnt ||
		    inode_in->i_sb != inode_out->i_sb)
			return -EXDEV;
	}

	if (same_inode)
		inode_lock(inode_in);
	else
		btrfs_double_inode_lock(inode_in, inode_out);

	/* don't make the dst file partly checksummed */
	if ((BTRFS_I(inode_in)->flags & BTRFS_INODE_NODATASUM) !=
	    (BTRFS_I(inode_out)->flags & BTRFS_INODE_NODATASUM)) {
		ret = -EINVAL;
		goto out_unlock;
	}

	/*
	 * Now that the inodes are locked, we need to start writeback ourselves
	 * and can not rely on the writeback from the VFS's generic helper
	 * generic_remap_file_range_prep() because:
	 *
	 * 1) For compression we must call filemap_fdatawrite_range() range
	 *    twice (btrfs_fdatawrite_range() does it for us), and the generic
	 *    helper only calls it once;
	 *
	 * 2) filemap_fdatawrite_range(), called by the generic helper only
	 *    waits for the writeback to complete, i.e. for IO to be done, and
	 *    not for the ordered extents to complete. We need to wait for them
	 *    to complete so that new file extent items are in the fs tree.
	 */
	if (*len == 0 && !(remap_flags & REMAP_FILE_DEDUP))
		wb_len = ALIGN(inode_in->i_size, bs) - ALIGN_DOWN(pos_in, bs);
	else
		wb_len = ALIGN(*len, bs);

	/*
	 * Since we don't lock ranges, wait for ongoing lockless dio writes (as
	 * any in progress could create its ordered extents after we wait for
	 * existing ordered extents below).
	 */
	inode_dio_wait(inode_in);
	if (!same_inode)
		inode_dio_wait(inode_out);

	ret = btrfs_wait_ordered_range(inode_in, ALIGN_DOWN(pos_in, bs),
				       wb_len);
	if (ret < 0)
		goto out_unlock;
	ret = btrfs_wait_ordered_range(inode_out, ALIGN_DOWN(pos_out, bs),
				       wb_len);
	if (ret < 0)
		goto out_unlock;

	ret = generic_remap_file_range_prep(file_in, pos_in, file_out, pos_out,
					    len, remap_flags);
	if (ret < 0 || *len == 0)
		goto out_unlock;

	return 0;

 out_unlock:
	if (same_inode)
		inode_unlock(inode_in);
	else
		btrfs_double_inode_unlock(inode_in, inode_out);

	return ret;
}