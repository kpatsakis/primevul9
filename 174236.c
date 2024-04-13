static int selinux_sb_clone_mnt_opts(const struct super_block *oldsb,
					struct super_block *newsb,
					unsigned long kern_flags,
					unsigned long *set_kern_flags)
{
	int rc = 0;
	const struct superblock_security_struct *oldsbsec = oldsb->s_security;
	struct superblock_security_struct *newsbsec = newsb->s_security;

	int set_fscontext =	(oldsbsec->flags & FSCONTEXT_MNT);
	int set_context =	(oldsbsec->flags & CONTEXT_MNT);
	int set_rootcontext =	(oldsbsec->flags & ROOTCONTEXT_MNT);

	/*
	 * if the parent was able to be mounted it clearly had no special lsm
	 * mount options.  thus we can safely deal with this superblock later
	 */
	if (!selinux_initialized(&selinux_state))
		return 0;

	/*
	 * Specifying internal flags without providing a place to
	 * place the results is not allowed.
	 */
	if (kern_flags && !set_kern_flags)
		return -EINVAL;

	/* how can we clone if the old one wasn't set up?? */
	BUG_ON(!(oldsbsec->flags & SE_SBINITIALIZED));

	/* if fs is reusing a sb, make sure that the contexts match */
	if (newsbsec->flags & SE_SBINITIALIZED) {
		if ((kern_flags & SECURITY_LSM_NATIVE_LABELS) && !set_context)
			*set_kern_flags |= SECURITY_LSM_NATIVE_LABELS;
		return selinux_cmp_sb_context(oldsb, newsb);
	}

	mutex_lock(&newsbsec->lock);

	newsbsec->flags = oldsbsec->flags;

	newsbsec->sid = oldsbsec->sid;
	newsbsec->def_sid = oldsbsec->def_sid;
	newsbsec->behavior = oldsbsec->behavior;

	if (newsbsec->behavior == SECURITY_FS_USE_NATIVE &&
		!(kern_flags & SECURITY_LSM_NATIVE_LABELS) && !set_context) {
		rc = security_fs_use(&selinux_state, newsb);
		if (rc)
			goto out;
	}

	if (kern_flags & SECURITY_LSM_NATIVE_LABELS && !set_context) {
		newsbsec->behavior = SECURITY_FS_USE_NATIVE;
		*set_kern_flags |= SECURITY_LSM_NATIVE_LABELS;
	}

	if (set_context) {
		u32 sid = oldsbsec->mntpoint_sid;

		if (!set_fscontext)
			newsbsec->sid = sid;
		if (!set_rootcontext) {
			struct inode_security_struct *newisec = backing_inode_security(newsb->s_root);
			newisec->sid = sid;
		}
		newsbsec->mntpoint_sid = sid;
	}
	if (set_rootcontext) {
		const struct inode_security_struct *oldisec = backing_inode_security(oldsb->s_root);
		struct inode_security_struct *newisec = backing_inode_security(newsb->s_root);

		newisec->sid = oldisec->sid;
	}

	sb_finish_set_opts(newsb);
out:
	mutex_unlock(&newsbsec->lock);
	return rc;
}