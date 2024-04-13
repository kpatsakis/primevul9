static struct buffer_head *bclean(handle_t *handle, struct super_block *sb,
				  ext4_fsblk_t blk)
{
	struct buffer_head *bh;
	int err;

	bh = sb_getblk(sb, blk);
	if (!bh)
		return ERR_PTR(-EIO);
	if ((err = ext4_journal_get_write_access(handle, bh))) {
		brelse(bh);
		bh = ERR_PTR(err);
	} else {
		memset(bh->b_data, 0, sb->s_blocksize);
		set_buffer_uptodate(bh);
	}

	return bh;
}