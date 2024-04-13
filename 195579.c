static struct buffer_head *ext4_get_bitmap(struct super_block *sb, __u64 block)
{
	struct buffer_head *bh = sb_getblk(sb, block);
	if (!bh)
		return NULL;

	if (bitmap_uptodate(bh))
		return bh;

	lock_buffer(bh);
	if (bh_submit_read(bh) < 0) {
		unlock_buffer(bh);
		brelse(bh);
		return NULL;
	}
	unlock_buffer(bh);

	return bh;
}