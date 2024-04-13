void ext4_xattr_inode_array_free(struct ext4_xattr_inode_array *ea_inode_array)
{
	int idx;

	if (ea_inode_array == NULL)
		return;

	for (idx = 0; idx < ea_inode_array->count; ++idx)
		iput(ea_inode_array->inodes[idx]);
	kfree(ea_inode_array);
}