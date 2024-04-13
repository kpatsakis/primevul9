static loff_t max_file_size(unsigned bits)
{
	loff_t result = (DEF_ADDRS_PER_INODE - F2FS_INLINE_XATTR_ADDRS);
	loff_t leaf_count = ADDRS_PER_BLOCK;

	/* two direct node blocks */
	result += (leaf_count * 2);

	/* two indirect node blocks */
	leaf_count *= NIDS_PER_BLOCK;
	result += (leaf_count * 2);

	/* one double indirect node block */
	leaf_count *= NIDS_PER_BLOCK;
	result += leaf_count;

	result <<= bits;
	return result;
}