static int selinux_set_mnt_opts(struct super_block *sb,
				void *mnt_opts,
				unsigned long kern_flags,
				unsigned long *set_kern_flags)
{
	const struct cred *cred = current_cred();
	struct superblock_security_struct *sbsec = sb->s_security;
	struct dentry *root = sbsec->sb->s_root;
	struct selinux_mnt_opts *opts = mnt_opts;
	struct inode_security_struct *root_isec;
	u32 fscontext_sid = 0, context_sid = 0, rootcontext_sid = 0;
	u32 defcontext_sid = 0;
	int rc = 0;

	mutex_lock(&sbsec->lock);

	if (!selinux_initialized(&selinux_state)) {
		if (!opts) {
			/* Defer initialization until selinux_complete_init,
			   after the initial policy is loaded and the security
			   server is ready to handle calls. */
			goto out;
		}
		rc = -EINVAL;
		pr_warn("SELinux: Unable to set superblock options "
			"before the security server is initialized\n");
		goto out;
	}
	if (kern_flags && !set_kern_flags) {
		/* Specifying internal flags without providing a place to
		 * place the results is not allowed */
		rc = -EINVAL;
		goto out;
	}

	/*
	 * Binary mount data FS will come through this function twice.  Once
	 * from an explicit call and once from the generic calls from the vfs.
	 * Since the generic VFS calls will not contain any security mount data
	 * we need to skip the double mount verification.
	 *
	 * This does open a hole in which we will not notice if the first
	 * mount using this sb set explict options and a second mount using
	 * this sb does not set any security options.  (The first options
	 * will be used for both mounts)
	 */
	if ((sbsec->flags & SE_SBINITIALIZED) && (sb->s_type->fs_flags & FS_BINARY_MOUNTDATA)
	    && !opts)
		goto out;

	root_isec = backing_inode_security_novalidate(root);

	/*
	 * parse the mount options, check if they are valid sids.
	 * also check if someone is trying to mount the same sb more
	 * than once with different security options.
	 */
	if (opts) {
		if (opts->fscontext) {
			rc = parse_sid(sb, opts->fscontext, &fscontext_sid);
			if (rc)
				goto out;
			if (bad_option(sbsec, FSCONTEXT_MNT, sbsec->sid,
					fscontext_sid))
				goto out_double_mount;
			sbsec->flags |= FSCONTEXT_MNT;
		}
		if (opts->context) {
			rc = parse_sid(sb, opts->context, &context_sid);
			if (rc)
				goto out;
			if (bad_option(sbsec, CONTEXT_MNT, sbsec->mntpoint_sid,
					context_sid))
				goto out_double_mount;
			sbsec->flags |= CONTEXT_MNT;
		}
		if (opts->rootcontext) {
			rc = parse_sid(sb, opts->rootcontext, &rootcontext_sid);
			if (rc)
				goto out;
			if (bad_option(sbsec, ROOTCONTEXT_MNT, root_isec->sid,
					rootcontext_sid))
				goto out_double_mount;
			sbsec->flags |= ROOTCONTEXT_MNT;
		}
		if (opts->defcontext) {
			rc = parse_sid(sb, opts->defcontext, &defcontext_sid);
			if (rc)
				goto out;
			if (bad_option(sbsec, DEFCONTEXT_MNT, sbsec->def_sid,
					defcontext_sid))
				goto out_double_mount;
			sbsec->flags |= DEFCONTEXT_MNT;
		}
	}

	if (sbsec->flags & SE_SBINITIALIZED) {
		/* previously mounted with options, but not on this attempt? */
		if ((sbsec->flags & SE_MNTMASK) && !opts)
			goto out_double_mount;
		rc = 0;
		goto out;
	}

	if (strcmp(sb->s_type->name, "proc") == 0)
		sbsec->flags |= SE_SBPROC | SE_SBGENFS;

	if (!strcmp(sb->s_type->name, "debugfs") ||
	    !strcmp(sb->s_type->name, "tracefs") ||
	    !strcmp(sb->s_type->name, "binderfs") ||
	    !strcmp(sb->s_type->name, "bpf") ||
	    !strcmp(sb->s_type->name, "pstore"))
		sbsec->flags |= SE_SBGENFS;

	if (!strcmp(sb->s_type->name, "sysfs") ||
	    !strcmp(sb->s_type->name, "cgroup") ||
	    !strcmp(sb->s_type->name, "cgroup2"))
		sbsec->flags |= SE_SBGENFS | SE_SBGENFS_XATTR;

	if (!sbsec->behavior) {
		/*
		 * Determine the labeling behavior to use for this
		 * filesystem type.
		 */
		rc = security_fs_use(&selinux_state, sb);
		if (rc) {
			pr_warn("%s: security_fs_use(%s) returned %d\n",
					__func__, sb->s_type->name, rc);
			goto out;
		}
	}

	/*
	 * If this is a user namespace mount and the filesystem type is not
	 * explicitly whitelisted, then no contexts are allowed on the command
	 * line and security labels must be ignored.
	 */
	if (sb->s_user_ns != &init_user_ns &&
	    strcmp(sb->s_type->name, "tmpfs") &&
	    strcmp(sb->s_type->name, "ramfs") &&
	    strcmp(sb->s_type->name, "devpts")) {
		if (context_sid || fscontext_sid || rootcontext_sid ||
		    defcontext_sid) {
			rc = -EACCES;
			goto out;
		}
		if (sbsec->behavior == SECURITY_FS_USE_XATTR) {
			sbsec->behavior = SECURITY_FS_USE_MNTPOINT;
			rc = security_transition_sid(&selinux_state,
						     current_sid(),
						     current_sid(),
						     SECCLASS_FILE, NULL,
						     &sbsec->mntpoint_sid);
			if (rc)
				goto out;
		}
		goto out_set_opts;
	}

	/* sets the context of the superblock for the fs being mounted. */
	if (fscontext_sid) {
		rc = may_context_mount_sb_relabel(fscontext_sid, sbsec, cred);
		if (rc)
			goto out;

		sbsec->sid = fscontext_sid;
	}

	/*
	 * Switch to using mount point labeling behavior.
	 * sets the label used on all file below the mountpoint, and will set
	 * the superblock context if not already set.
	 */
	if (kern_flags & SECURITY_LSM_NATIVE_LABELS && !context_sid) {
		sbsec->behavior = SECURITY_FS_USE_NATIVE;
		*set_kern_flags |= SECURITY_LSM_NATIVE_LABELS;
	}

	if (context_sid) {
		if (!fscontext_sid) {
			rc = may_context_mount_sb_relabel(context_sid, sbsec,
							  cred);
			if (rc)
				goto out;
			sbsec->sid = context_sid;
		} else {
			rc = may_context_mount_inode_relabel(context_sid, sbsec,
							     cred);
			if (rc)
				goto out;
		}
		if (!rootcontext_sid)
			rootcontext_sid = context_sid;

		sbsec->mntpoint_sid = context_sid;
		sbsec->behavior = SECURITY_FS_USE_MNTPOINT;
	}

	if (rootcontext_sid) {
		rc = may_context_mount_inode_relabel(rootcontext_sid, sbsec,
						     cred);
		if (rc)
			goto out;

		root_isec->sid = rootcontext_sid;
		root_isec->initialized = LABEL_INITIALIZED;
	}

	if (defcontext_sid) {
		if (sbsec->behavior != SECURITY_FS_USE_XATTR &&
			sbsec->behavior != SECURITY_FS_USE_NATIVE) {
			rc = -EINVAL;
			pr_warn("SELinux: defcontext option is "
			       "invalid for this filesystem type\n");
			goto out;
		}

		if (defcontext_sid != sbsec->def_sid) {
			rc = may_context_mount_inode_relabel(defcontext_sid,
							     sbsec, cred);
			if (rc)
				goto out;
		}

		sbsec->def_sid = defcontext_sid;
	}

out_set_opts:
	rc = sb_finish_set_opts(sb);
out:
	mutex_unlock(&sbsec->lock);
	return rc;
out_double_mount:
	rc = -EINVAL;
	pr_warn("SELinux: mount invalid.  Same superblock, different "
	       "security settings for (dev %s, type %s)\n", sb->s_id,
	       sb->s_type->name);
	goto out;
}