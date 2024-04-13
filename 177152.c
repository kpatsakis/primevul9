ext4_xattr_inode_hash(struct ext4_sb_info *sbi, const void *buffer, size_t size)
{
	return ext4_chksum(sbi, sbi->s_csum_seed, buffer, size);
}