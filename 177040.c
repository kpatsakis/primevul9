const char *btrfs_super_csum_name(u16 csum_type)
{
	/* csum type is validated at mount time */
	return btrfs_csums[csum_type].name;
}