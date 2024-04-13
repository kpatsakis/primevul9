static int _nfs4_proc_secinfo(struct inode *dir, const struct qstr *name, struct nfs4_secinfo_flavors *flavors)
{
	int status;
	struct nfs4_secinfo_arg args = {
		.dir_fh = NFS_FH(dir),
		.name   = name,
	};
	struct nfs4_secinfo_res res = {
		.flavors     = flavors,
	};
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_SECINFO],
		.rpc_argp = &args,
		.rpc_resp = &res,
	};

	dprintk("NFS call  secinfo %s\n", name->name);
	status = nfs4_call_sync(NFS_SERVER(dir)->client, NFS_SERVER(dir), &msg, &args.seq_args, &res.seq_res, 0);
	dprintk("NFS reply  secinfo: %d\n", status);
	return status;
}