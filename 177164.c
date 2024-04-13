static int ext4_xattr_inode_read(struct inode *ea_inode, void *buf, size_t size)
{
	int blocksize = 1 << ea_inode->i_blkbits;
	int bh_count = (size + blocksize - 1) >> ea_inode->i_blkbits;
	int tail_size = (size % blocksize) ?: blocksize;
	struct buffer_head *bhs_inline[8];
	struct buffer_head **bhs = bhs_inline;
	int i, ret;

	if (bh_count > ARRAY_SIZE(bhs_inline)) {
		bhs = kmalloc_array(bh_count, sizeof(*bhs), GFP_NOFS);
		if (!bhs)
			return -ENOMEM;
	}

	ret = ext4_bread_batch(ea_inode, 0 /* block */, bh_count,
			       true /* wait */, bhs);
	if (ret)
		goto free_bhs;

	for (i = 0; i < bh_count; i++) {
		/* There shouldn't be any holes in ea_inode. */
		if (!bhs[i]) {
			ret = -EFSCORRUPTED;
			goto put_bhs;
		}
		memcpy((char *)buf + blocksize * i, bhs[i]->b_data,
		       i < bh_count - 1 ? blocksize : tail_size);
	}
	ret = 0;
put_bhs:
	for (i = 0; i < bh_count; i++)
		brelse(bhs[i]);
free_bhs:
	if (bhs != bhs_inline)
		kfree(bhs);
	return ret;
}