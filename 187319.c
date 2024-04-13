int nfs4_proc_getdevicelist(struct nfs_server *server,
			    const struct nfs_fh *fh,
			    struct pnfs_devicelist *devlist)
{
	struct nfs4_exception exception = { };
	int err;

	do {
		err = nfs4_handle_exception(server,
				_nfs4_getdevicelist(server, fh, devlist),
				&exception);
	} while (exception.retry);

	dprintk("%s: err=%d, num_devs=%u\n", __func__,
		err, devlist->num_devs);

	return err;
}