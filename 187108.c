nfs41_proc_secinfo_no_name(struct nfs_server *server, struct nfs_fh *fhandle,
			   struct nfs_fsinfo *info, struct nfs4_secinfo_flavors *flavors)
{
	struct nfs4_exception exception = { };
	int err;
	do {
		err = _nfs41_proc_secinfo_no_name(server, fhandle, info, flavors);
		switch (err) {
		case 0:
		case -NFS4ERR_WRONGSEC:
		case -NFS4ERR_NOTSUPP:
			goto out;
		default:
			err = nfs4_handle_exception(server, err, &exception);
		}
	} while (exception.retry);
out:
	return err;
}