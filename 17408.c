static int path_init(int dfd, const char *name, unsigned int flags,
		     struct nameidata *nd, struct file **fp)
{
	int retval = 0;

	nd->last_type = LAST_ROOT; /* if there are only slashes... */
	nd->flags = flags | LOOKUP_JUMPED;
	nd->depth = 0;
	if (flags & LOOKUP_ROOT) {
		struct inode *inode = nd->root.dentry->d_inode;
		if (*name) {
			if (!can_lookup(inode))
				return -ENOTDIR;
			retval = inode_permission(inode, MAY_EXEC);
			if (retval)
				return retval;
		}
		nd->path = nd->root;
		nd->inode = inode;
		if (flags & LOOKUP_RCU) {
			lock_rcu_walk();
			nd->seq = __read_seqcount_begin(&nd->path.dentry->d_seq);
		} else {
			path_get(&nd->path);
		}
		return 0;
	}

	nd->root.mnt = NULL;

	if (*name=='/') {
		if (flags & LOOKUP_RCU) {
			lock_rcu_walk();
			set_root_rcu(nd);
		} else {
			set_root(nd);
			path_get(&nd->root);
		}
		nd->path = nd->root;
	} else if (dfd == AT_FDCWD) {
		if (flags & LOOKUP_RCU) {
			struct fs_struct *fs = current->fs;
			unsigned seq;

			lock_rcu_walk();

			do {
				seq = read_seqcount_begin(&fs->seq);
				nd->path = fs->pwd;
				nd->seq = __read_seqcount_begin(&nd->path.dentry->d_seq);
			} while (read_seqcount_retry(&fs->seq, seq));
		} else {
			get_fs_pwd(current->fs, &nd->path);
		}
	} else {
		/* Caller must check execute permissions on the starting path component */
		struct fd f = fdget_raw(dfd);
		struct dentry *dentry;

		if (!f.file)
			return -EBADF;

		dentry = f.file->f_path.dentry;

		if (*name) {
			if (!can_lookup(dentry->d_inode)) {
				fdput(f);
				return -ENOTDIR;
			}
		}

		nd->path = f.file->f_path;
		if (flags & LOOKUP_RCU) {
			if (f.need_put)
				*fp = f.file;
			nd->seq = __read_seqcount_begin(&nd->path.dentry->d_seq);
			lock_rcu_walk();
		} else {
			path_get(&nd->path);
			fdput(f);
		}
	}

	nd->inode = nd->path.dentry->d_inode;
	return 0;
}