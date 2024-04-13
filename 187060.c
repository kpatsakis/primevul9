static int nfs4_proc_mknod(struct inode *dir, struct dentry *dentry,
		struct iattr *sattr, dev_t rdev)
{
	struct nfs4_exception exception = { };
	int err;

	sattr->ia_mode &= ~current_umask();
	do {
		err = nfs4_handle_exception(NFS_SERVER(dir),
				_nfs4_proc_mknod(dir, dentry, sattr, rdev),
				&exception);
	} while (exception.retry);
	return err;
}