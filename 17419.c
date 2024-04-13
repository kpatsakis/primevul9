static int atomic_open(struct nameidata *nd, struct dentry *dentry,
			struct path *path, struct file *file,
			const struct open_flags *op,
			bool got_write, bool need_lookup,
			int *opened)
{
	struct inode *dir =  nd->path.dentry->d_inode;
	unsigned open_flag = open_to_namei_flags(op->open_flag);
	umode_t mode;
	int error;
	int acc_mode;
	int create_error = 0;
	struct dentry *const DENTRY_NOT_SET = (void *) -1UL;

	BUG_ON(dentry->d_inode);

	/* Don't create child dentry for a dead directory. */
	if (unlikely(IS_DEADDIR(dir))) {
		error = -ENOENT;
		goto out;
	}

	mode = op->mode;
	if ((open_flag & O_CREAT) && !IS_POSIXACL(dir))
		mode &= ~current_umask();

	if ((open_flag & (O_EXCL | O_CREAT)) == (O_EXCL | O_CREAT)) {
		open_flag &= ~O_TRUNC;
		*opened |= FILE_CREATED;
	}

	/*
	 * Checking write permission is tricky, bacuse we don't know if we are
	 * going to actually need it: O_CREAT opens should work as long as the
	 * file exists.  But checking existence breaks atomicity.  The trick is
	 * to check access and if not granted clear O_CREAT from the flags.
	 *
	 * Another problem is returing the "right" error value (e.g. for an
	 * O_EXCL open we want to return EEXIST not EROFS).
	 */
	if (((open_flag & (O_CREAT | O_TRUNC)) ||
	    (open_flag & O_ACCMODE) != O_RDONLY) && unlikely(!got_write)) {
		if (!(open_flag & O_CREAT)) {
			/*
			 * No O_CREATE -> atomicity not a requirement -> fall
			 * back to lookup + open
			 */
			goto no_open;
		} else if (open_flag & (O_EXCL | O_TRUNC)) {
			/* Fall back and fail with the right error */
			create_error = -EROFS;
			goto no_open;
		} else {
			/* No side effects, safe to clear O_CREAT */
			create_error = -EROFS;
			open_flag &= ~O_CREAT;
		}
	}

	if (open_flag & O_CREAT) {
		error = may_o_create(&nd->path, dentry, mode);
		if (error) {
			create_error = error;
			if (open_flag & O_EXCL)
				goto no_open;
			open_flag &= ~O_CREAT;
		}
	}

	if (nd->flags & LOOKUP_DIRECTORY)
		open_flag |= O_DIRECTORY;

	file->f_path.dentry = DENTRY_NOT_SET;
	file->f_path.mnt = nd->path.mnt;
	error = dir->i_op->atomic_open(dir, dentry, file, open_flag, mode,
				      opened);
	if (error < 0) {
		if (create_error && error == -ENOENT)
			error = create_error;
		goto out;
	}

	acc_mode = op->acc_mode;
	if (*opened & FILE_CREATED) {
		fsnotify_create(dir, dentry);
		acc_mode = MAY_OPEN;
	}

	if (error) {	/* returned 1, that is */
		if (WARN_ON(file->f_path.dentry == DENTRY_NOT_SET)) {
			error = -EIO;
			goto out;
		}
		if (file->f_path.dentry) {
			dput(dentry);
			dentry = file->f_path.dentry;
		}
		if (create_error && dentry->d_inode == NULL) {
			error = create_error;
			goto out;
		}
		goto looked_up;
	}

	/*
	 * We didn't have the inode before the open, so check open permission
	 * here.
	 */
	error = may_open(&file->f_path, acc_mode, open_flag);
	if (error)
		fput(file);

out:
	dput(dentry);
	return error;

no_open:
	if (need_lookup) {
		dentry = lookup_real(dir, dentry, nd->flags);
		if (IS_ERR(dentry))
			return PTR_ERR(dentry);

		if (create_error) {
			int open_flag = op->open_flag;

			error = create_error;
			if ((open_flag & O_EXCL)) {
				if (!dentry->d_inode)
					goto out;
			} else if (!dentry->d_inode) {
				goto out;
			} else if ((open_flag & O_TRUNC) &&
				   S_ISREG(dentry->d_inode->i_mode)) {
				goto out;
			}
			/* will fail later, go on to get the right error */
		}
	}
looked_up:
	path->dentry = dentry;
	path->mnt = nd->path.mnt;
	return 1;
}