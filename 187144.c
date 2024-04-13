static int nfs4_proc_pathconf(struct nfs_server *server, struct nfs_fh *fhandle,
		struct nfs_pathconf *pathconf)
{
	struct nfs4_exception exception = { };
	int err;

	do {
		err = nfs4_handle_exception(server,
				_nfs4_proc_pathconf(server, fhandle, pathconf),
				&exception);
	} while (exception.retry);
	return err;
}