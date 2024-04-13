static int nfs4_lookup_root_sec(struct nfs_server *server, struct nfs_fh *fhandle,
				struct nfs_fsinfo *info, rpc_authflavor_t flavor)
{
	struct rpc_auth *auth;
	int ret;

	auth = rpcauth_create(flavor, server->client);
	if (!auth) {
		ret = -EIO;
		goto out;
	}
	ret = nfs4_lookup_root(server, fhandle, info);
out:
	return ret;
}