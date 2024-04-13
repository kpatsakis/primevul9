}

static inline bool f2fs_force_buffered_io(struct inode *inode,
				struct kiocb *iocb, struct iov_iter *iter)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	int rw = iov_iter_rw(iter);

	if (f2fs_post_read_required(inode))
		return true;
	if (f2fs_is_multi_device(sbi))
		return true;
	/*
	 * for blkzoned device, fallback direct IO to buffered IO, so
	 * all IOs can be serialized by log-structured write.
	 */
	if (f2fs_sb_has_blkzoned(sbi))
		return true;
	if (test_opt(sbi, LFS) && (rw == WRITE) &&
				block_unaligned_IO(inode, iocb, iter))
		return true;
	if (is_sbi_flag_set(F2FS_I_SB(inode), SBI_CP_DISABLED) &&
					!(inode->i_flags & S_SWAPFILE))
		return true;
