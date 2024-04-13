SYSCALL_DEFINE3(faccessat, int, dfd, const char __user *, filename, int, mode)
{
	const struct cred *old_cred;
	struct cred *override_cred;
	struct path path;
	struct inode *inode;
	int res;
	unsigned int lookup_flags = LOOKUP_FOLLOW;

	if (mode & ~S_IRWXO)	/* where's F_OK, X_OK, W_OK, R_OK? */
		return -EINVAL;

	override_cred = prepare_creds();
	if (!override_cred)
		return -ENOMEM;

	override_cred->fsuid = override_cred->uid;
	override_cred->fsgid = override_cred->gid;

	if (!issecure(SECURE_NO_SETUID_FIXUP)) {
		/* Clear the capabilities if we switch to a non-root user */
		kuid_t root_uid = make_kuid(override_cred->user_ns, 0);
		if (!uid_eq(override_cred->uid, root_uid))
			cap_clear(override_cred->cap_effective);
		else
			override_cred->cap_effective =
				override_cred->cap_permitted;
	}

	old_cred = override_creds(override_cred);
retry:
	res = user_path_at(dfd, filename, lookup_flags, &path);
	if (res)
		goto out;

	inode = path.dentry->d_inode;

	if ((mode & MAY_EXEC) && S_ISREG(inode->i_mode)) {
		/*
		 * MAY_EXEC on regular files is denied if the fs is mounted
		 * with the "noexec" flag.
		 */
		res = -EACCES;
		if (path.mnt->mnt_flags & MNT_NOEXEC)
			goto out_path_release;
	}

	res = inode_permission(inode, mode | MAY_ACCESS);
	/* SuS v2 requires we report a read only fs too */
	if (res || !(mode & S_IWOTH) || special_file(inode->i_mode))
		goto out_path_release;
	/*
	 * This is a rare case where using __mnt_is_readonly()
	 * is OK without a mnt_want/drop_write() pair.  Since
	 * no actual write to the fs is performed here, we do
	 * not need to telegraph to that to anyone.
	 *
	 * By doing this, we accept that this access is
	 * inherently racy and know that the fs may change
	 * state before we even see this result.
	 */
	if (__mnt_is_readonly(path.mnt))
		res = -EROFS;

out_path_release:
	path_put(&path);
	if (retry_estale(res, lookup_flags)) {
		lookup_flags |= LOOKUP_REVAL;
		goto retry;
	}
out:
	revert_creds(old_cred);
	put_cred(override_cred);
	return res;
}