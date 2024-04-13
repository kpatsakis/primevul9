int f2fs_commit_super(struct f2fs_sb_info *sbi, bool recover)
{
	struct buffer_head *sbh = sbi->raw_super_buf;
	struct buffer_head *bh;
	int err;

	/* write back-up superblock first */
	bh = sb_getblk(sbi->sb, sbh->b_blocknr ? 0 : 1);
	if (!bh)
		return -EIO;

	lock_buffer(bh);
	memcpy(bh->b_data, sbh->b_data, sbh->b_size);
	WARN_ON(sbh->b_size != F2FS_BLKSIZE);
	set_buffer_uptodate(bh);
	set_buffer_dirty(bh);
	unlock_buffer(bh);

	/* it's rare case, we can do fua all the time */
	err = __sync_dirty_buffer(bh, WRITE_FLUSH_FUA);
	brelse(bh);

	/* if we are in recovery path, skip writing valid superblock */
	if (recover || err)
		return err;

	/* write current valid superblock */
	lock_buffer(sbh);
	set_buffer_dirty(sbh);
	unlock_buffer(sbh);

	return __sync_dirty_buffer(sbh, WRITE_FLUSH_FUA);
}