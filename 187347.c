static int nfs4_proc_access(struct inode *inode, struct nfs_access_entry *entry)
{
	struct nfs4_exception exception = { };
	int err;
	do {
		err = nfs4_handle_exception(NFS_SERVER(inode),
				_nfs4_proc_access(inode, entry),
				&exception);
	} while (exception.retry);
	return err;
}