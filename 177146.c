static int ext4_xattr_inode_alloc_quota(struct inode *inode, size_t len)
{
	int err;

	err = dquot_alloc_inode(inode);
	if (err)
		return err;
	err = dquot_alloc_space_nodirty(inode, round_up_cluster(inode, len));
	if (err)
		dquot_free_inode(inode);
	return err;
}