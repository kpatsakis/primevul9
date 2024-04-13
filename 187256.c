static int _nfs4_do_setattr(struct inode *inode, struct rpc_cred *cred,
			    struct nfs_fattr *fattr, struct iattr *sattr,
			    struct nfs4_state *state)
{
	struct nfs_server *server = NFS_SERVER(inode);
        struct nfs_setattrargs  arg = {
                .fh             = NFS_FH(inode),
                .iap            = sattr,
		.server		= server,
		.bitmask = server->attr_bitmask,
        };
        struct nfs_setattrres  res = {
		.fattr		= fattr,
		.server		= server,
        };
        struct rpc_message msg = {
		.rpc_proc	= &nfs4_procedures[NFSPROC4_CLNT_SETATTR],
		.rpc_argp	= &arg,
		.rpc_resp	= &res,
		.rpc_cred	= cred,
        };
	unsigned long timestamp = jiffies;
	int status;

	nfs_fattr_init(fattr);

	if (state != NULL) {
		nfs4_select_rw_stateid(&arg.stateid, state, FMODE_WRITE,
				current->files, current->tgid);
	} else if (nfs4_copy_delegation_stateid(&arg.stateid, inode,
				FMODE_WRITE)) {
		/* Use that stateid */
	} else
		nfs4_stateid_copy(&arg.stateid, &zero_stateid);

	status = nfs4_call_sync(server->client, server, &msg, &arg.seq_args, &res.seq_res, 1);
	if (status == 0 && state != NULL)
		renew_lease(server, timestamp);
	return status;
}