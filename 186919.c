int nfs4_proc_secinfo(struct inode *dir, const struct qstr *name,
		      struct nfs4_secinfo_flavors *flavors)
{
	struct nfs4_exception exception = { };
	int err;
	do {
		err = nfs4_handle_exception(NFS_SERVER(dir),
				_nfs4_proc_secinfo(dir, name, flavors),
				&exception);
	} while (exception.retry);
	return err;
}