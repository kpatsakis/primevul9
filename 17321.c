static inline int may_follow_link(struct path *link, struct nameidata *nd)
{
	const struct inode *inode;
	const struct inode *parent;

	if (!sysctl_protected_symlinks)
		return 0;

	/* Allowed if owner and follower match. */
	inode = link->dentry->d_inode;
	if (uid_eq(current_cred()->fsuid, inode->i_uid))
		return 0;

	/* Allowed if parent directory not sticky and world-writable. */
	parent = nd->path.dentry->d_inode;
	if ((parent->i_mode & (S_ISVTX|S_IWOTH)) != (S_ISVTX|S_IWOTH))
		return 0;

	/* Allowed if parent directory and link owner match. */
	if (uid_eq(parent->i_uid, inode->i_uid))
		return 0;

	audit_log_link_denied("follow_link", link);
	path_put_conditional(link, nd);
	path_put(&nd->path);
	return -EACCES;
}