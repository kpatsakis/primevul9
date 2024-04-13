static int nfs4_proc_remove(struct inode *dir, struct qstr *name)
{
	struct nfs4_exception exception = { };
	int err;
	do {
		err = nfs4_handle_exception(NFS_SERVER(dir),
				_nfs4_proc_remove(dir, name),
				&exception);
	} while (exception.retry);
	return err;
}