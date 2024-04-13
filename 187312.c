int nfs4_proc_get_rootfh(struct nfs_server *server, struct nfs_fh *fhandle,
			 struct nfs_fsinfo *info)
{
	int minor_version = server->nfs_client->cl_minorversion;
	int status = nfs4_lookup_root(server, fhandle, info);
	if ((status == -NFS4ERR_WRONGSEC) && !(server->flags & NFS_MOUNT_SECFLAVOUR))
		/*
		 * A status of -NFS4ERR_WRONGSEC will be mapped to -EPERM
		 * by nfs4_map_errors() as this function exits.
		 */
		status = nfs_v4_minor_ops[minor_version]->find_root_sec(server, fhandle, info);
	if (status == 0)
		status = nfs4_server_capabilities(server, fhandle);
	if (status == 0)
		status = nfs4_do_fsinfo(server, fhandle, info);
	return nfs4_map_errors(status);
}