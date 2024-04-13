ext4_xattr_get(struct inode *inode, int name_index, const char *name,
	       void *buffer, size_t buffer_size)
{
	int error;

	if (unlikely(ext4_forced_shutdown(EXT4_SB(inode->i_sb))))
		return -EIO;

	if (strlen(name) > 255)
		return -ERANGE;

	down_read(&EXT4_I(inode)->xattr_sem);
	error = ext4_xattr_ibody_get(inode, name_index, name, buffer,
				     buffer_size);
	if (error == -ENODATA)
		error = ext4_xattr_block_get(inode, name_index, name, buffer,
					     buffer_size);
	up_read(&EXT4_I(inode)->xattr_sem);
	return error;
}