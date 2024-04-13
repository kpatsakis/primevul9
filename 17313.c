static int sb_permission(struct super_block *sb, struct inode *inode, int mask)
{
	if (unlikely(mask & MAY_WRITE)) {
		umode_t mode = inode->i_mode;

		/* Nobody gets write access to a read-only fs. */
		if ((sb->s_flags & MS_RDONLY) &&
		    (S_ISREG(mode) || S_ISDIR(mode) || S_ISLNK(mode)))
			return -EROFS;
	}
	return 0;
}