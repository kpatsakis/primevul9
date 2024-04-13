int btrfs_super_csum_size(const struct btrfs_super_block *s)
{
	u16 t = btrfs_super_csum_type(s);
	/*
	 * csum type is validated at mount time
	 */
	return btrfs_csums[t].size;
}