static struct inode *ext4_nfs_get_inode(struct super_block *sb,
					u64 ino, u32 generation)
{
	struct inode *inode;

	/*
	 * Currently we don't know the generation for parent directory, so
	 * a generation of 0 means "accept any"
	 */
	inode = ext4_iget(sb, ino, EXT4_IGET_HANDLE);
	if (IS_ERR(inode))
		return ERR_CAST(inode);
	if (generation && inode->i_generation != generation) {
		iput(inode);
		return ERR_PTR(-ESTALE);
	}

	return inode;
}