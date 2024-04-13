static int nfs4_proc_rename(struct inode *old_dir, struct qstr *old_name,
		struct inode *new_dir, struct qstr *new_name)
{
	struct nfs4_exception exception = { };
	int err;
	do {
		err = nfs4_handle_exception(NFS_SERVER(old_dir),
				_nfs4_proc_rename(old_dir, old_name,
					new_dir, new_name),
				&exception);
	} while (exception.retry);
	return err;
}