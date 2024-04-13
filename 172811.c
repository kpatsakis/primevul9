static int ext4_drop_inode(struct inode *inode)
{
	int drop = generic_drop_inode(inode);

	if (!drop)
		drop = fscrypt_drop_inode(inode);

	trace_ext4_drop_inode(inode, drop);
	return drop;
}