static int nfs4_proc_getattr(struct nfs_server *server, struct nfs_fh *fhandle, struct nfs_fattr *fattr)
{
	struct nfs4_exception exception = { };
	int err;
	do {
		err = nfs4_handle_exception(server,
				_nfs4_proc_getattr(server, fhandle, fattr),
				&exception);
	} while (exception.retry);
	return err;
}