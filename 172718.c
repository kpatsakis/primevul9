static int btrfs_extent_same_range(struct inode *src, u64 loff, u64 olen,
				   struct inode *dst, u64 dst_loff)
{
	u64 bs = BTRFS_I(src)->root->fs_info->sb->s_blocksize;
	int ret;
	u64 len = olen;

	if (loff + len == src->i_size)
		len = ALIGN(src->i_size, bs) - loff;
	/*
	 * For same inode case we don't want our length pushed out past i_size
	 * as comparing that data range makes no sense.
	 *
	 * This effectively means we require aligned extents for the single
	 * inode case, whereas the other cases allow an unaligned length so long
	 * as it ends at i_size.
	 */
	if (dst == src && len != olen)
		return -EINVAL;

	/*
	 * Lock destination range to serialize with concurrent readpages() and
	 * source range to serialize with relocation.
	 */
	btrfs_double_extent_lock(src, loff, dst, dst_loff, len);
	ret = btrfs_clone(src, dst, loff, olen, len, dst_loff, 1);
	btrfs_double_extent_unlock(src, loff, dst, dst_loff, len);

	return ret;
}