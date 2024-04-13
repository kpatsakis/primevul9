static int selinux_inode_setsecurity(struct inode *inode, const char *name,
				     const void *value, size_t size, int flags)
{
	struct inode_security_struct *isec = inode_security_novalidate(inode);
	struct superblock_security_struct *sbsec = inode->i_sb->s_security;
	u32 newsid;
	int rc;

	if (strcmp(name, XATTR_SELINUX_SUFFIX))
		return -EOPNOTSUPP;

	if (!(sbsec->flags & SBLABEL_MNT))
		return -EOPNOTSUPP;

	if (!value || !size)
		return -EACCES;

	rc = security_context_to_sid(&selinux_state, value, size, &newsid,
				     GFP_KERNEL);
	if (rc)
		return rc;

	spin_lock(&isec->lock);
	isec->sclass = inode_mode_to_security_class(inode->i_mode);
	isec->sid = newsid;
	isec->initialized = LABEL_INITIALIZED;
	spin_unlock(&isec->lock);
	return 0;
}