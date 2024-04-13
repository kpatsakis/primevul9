ext4_sb_bread(struct super_block *sb, sector_t block, int op_flags)
{
	struct buffer_head *bh = sb_getblk(sb, block);

	if (bh == NULL)
		return ERR_PTR(-ENOMEM);
	if (buffer_uptodate(bh))
		return bh;
	ll_rw_block(REQ_OP_READ, REQ_META | op_flags, 1, &bh);
	wait_on_buffer(bh);
	if (buffer_uptodate(bh))
		return bh;
	put_bh(bh);
	return ERR_PTR(-EIO);
}