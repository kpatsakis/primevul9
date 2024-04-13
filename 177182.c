ext4_listxattr(struct dentry *dentry, char *buffer, size_t buffer_size)
{
	int ret, ret2;

	down_read(&EXT4_I(d_inode(dentry))->xattr_sem);
	ret = ret2 = ext4_xattr_ibody_list(dentry, buffer, buffer_size);
	if (ret < 0)
		goto errout;
	if (buffer) {
		buffer += ret;
		buffer_size -= ret;
	}
	ret = ext4_xattr_block_list(dentry, buffer, buffer_size);
	if (ret < 0)
		goto errout;
	ret += ret2;
errout:
	up_read(&EXT4_I(d_inode(dentry))->xattr_sem);
	return ret;
}