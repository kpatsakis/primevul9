static int nfs4_proc_readlink(struct inode *inode, struct page *page,
		unsigned int pgbase, unsigned int pglen)
{
	struct nfs4_exception exception = { };
	int err;
	do {
		err = nfs4_handle_exception(NFS_SERVER(inode),
				_nfs4_proc_readlink(inode, page, pgbase, pglen),
				&exception);
	} while (exception.retry);
	return err;
}