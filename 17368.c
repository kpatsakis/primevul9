static inline int __get_file_write_access(struct inode *inode,
					  struct vfsmount *mnt)
{
	int error;
	error = get_write_access(inode);
	if (error)
		return error;
	/*
	 * Do not take mount writer counts on
	 * special files since no writes to
	 * the mount itself will occur.
	 */
	if (!special_file(inode->i_mode)) {
		/*
		 * Balanced in __fput()
		 */
		error = __mnt_want_write(mnt);
		if (error)
			put_write_access(inode);
	}
	return error;
}