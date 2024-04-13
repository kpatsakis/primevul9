static int selinux_inode_init_security(struct inode *inode, struct inode *dir,
				       const struct qstr *qstr,
				       const char **name,
				       void **value, size_t *len)
{
	const struct task_security_struct *tsec = selinux_cred(current_cred());
	struct superblock_security_struct *sbsec;
	u32 newsid, clen;
	int rc;
	char *context;

	sbsec = dir->i_sb->s_security;

	newsid = tsec->create_sid;

	rc = selinux_determine_inode_label(tsec, dir, qstr,
		inode_mode_to_security_class(inode->i_mode),
		&newsid);
	if (rc)
		return rc;

	/* Possibly defer initialization to selinux_complete_init. */
	if (sbsec->flags & SE_SBINITIALIZED) {
		struct inode_security_struct *isec = selinux_inode(inode);
		isec->sclass = inode_mode_to_security_class(inode->i_mode);
		isec->sid = newsid;
		isec->initialized = LABEL_INITIALIZED;
	}

	if (!selinux_initialized(&selinux_state) ||
	    !(sbsec->flags & SBLABEL_MNT))
		return -EOPNOTSUPP;

	if (name)
		*name = XATTR_SELINUX_SUFFIX;

	if (value && len) {
		rc = security_sid_to_context_force(&selinux_state, newsid,
						   &context, &clen);
		if (rc)
			return rc;
		*value = context;
		*len = clen;
	}

	return 0;
}