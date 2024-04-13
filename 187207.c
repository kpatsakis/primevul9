static int nfs4_proc_set_acl(struct inode *inode, const void *buf, size_t buflen)
{
	struct nfs4_exception exception = { };
	int err;
	do {
		err = nfs4_handle_exception(NFS_SERVER(inode),
				__nfs4_proc_set_acl(inode, buf, buflen),
				&exception);
	} while (exception.retry);
	return err;
}