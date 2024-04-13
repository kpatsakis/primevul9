}

static inline int block_unaligned_IO(struct inode *inode,
				struct kiocb *iocb, struct iov_iter *iter)
{
	unsigned int i_blkbits = READ_ONCE(inode->i_blkbits);
	unsigned int blocksize_mask = (1 << i_blkbits) - 1;
	loff_t offset = iocb->ki_pos;
	unsigned long align = offset | iov_iter_alignment(iter);
