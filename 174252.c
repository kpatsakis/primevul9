static int selinux_inode_removexattr(struct dentry *dentry, const char *name)
{
	if (strcmp(name, XATTR_NAME_SELINUX)) {
		int rc = cap_inode_removexattr(dentry, name);
		if (rc)
			return rc;

		/* Not an attribute we recognize, so just check the
		   ordinary setattr permission. */
		return dentry_has_perm(current_cred(), dentry, FILE__SETATTR);
	}

	/* No one is allowed to remove a SELinux security label.
	   You can change the label, but all data must be labeled. */
	return -EACCES;
}