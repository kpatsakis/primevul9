static int nfs4_do_fsinfo(struct nfs_server *server, struct nfs_fh *fhandle, struct nfs_fsinfo *fsinfo)
{
	struct nfs4_exception exception = { };
	int err;

	do {
		err = nfs4_handle_exception(server,
				_nfs4_do_fsinfo(server, fhandle, fsinfo),
				&exception);
	} while (exception.retry);
	return err;
}