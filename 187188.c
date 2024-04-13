static int nfs4_find_root_sec(struct nfs_server *server, struct nfs_fh *fhandle,
			      struct nfs_fsinfo *info)
{
	int i, len, status = 0;
	rpc_authflavor_t flav_array[NFS_MAX_SECFLAVORS];

	len = rpcauth_list_flavors(flav_array, ARRAY_SIZE(flav_array));
	BUG_ON(len < 0);

	for (i = 0; i < len; i++) {
		/* AUTH_UNIX is the default flavor if none was specified,
		 * thus has already been tried. */
		if (flav_array[i] == RPC_AUTH_UNIX)
			continue;

		status = nfs4_lookup_root_sec(server, fhandle, info, flav_array[i]);
		if (status == -NFS4ERR_WRONGSEC || status == -EACCES)
			continue;
		break;
	}
	/*
	 * -EACCESS could mean that the user doesn't have correct permissions
	 * to access the mount.  It could also mean that we tried to mount
	 * with a gss auth flavor, but rpc.gssd isn't running.  Either way,
	 * existing mount programs don't handle -EACCES very well so it should
	 * be mapped to -EPERM instead.
	 */
	if (status == -EACCES)
		status = -EPERM;
	return status;
}