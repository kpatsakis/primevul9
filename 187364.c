static void nfs4_proc_unlink_setup(struct rpc_message *msg, struct inode *dir)
{
	struct nfs_server *server = NFS_SERVER(dir);
	struct nfs_removeargs *args = msg->rpc_argp;
	struct nfs_removeres *res = msg->rpc_resp;

	res->server = server;
	msg->rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_REMOVE];
	nfs41_init_sequence(&args->seq_args, &res->seq_res, 1);
}