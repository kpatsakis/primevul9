}

static inline int allow_outplace_dio(struct inode *inode,
				struct kiocb *iocb, struct iov_iter *iter)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	int rw = iov_iter_rw(iter);

	return (test_opt(sbi, LFS) && (rw == WRITE) &&