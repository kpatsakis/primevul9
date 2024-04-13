int nfs4_proc_getdeviceinfo(struct nfs_server *server, struct pnfs_device *pdev)
{
	struct nfs4_exception exception = { };
	int err;

	do {
		err = nfs4_handle_exception(server,
					_nfs4_proc_getdeviceinfo(server, pdev),
					&exception);
	} while (exception.retry);
	return err;
}