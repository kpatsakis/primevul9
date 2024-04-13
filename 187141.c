static struct nfs4_createdata *nfs4_alloc_createdata(struct inode *dir,
		struct qstr *name, struct iattr *sattr, u32 ftype)
{
	struct nfs4_createdata *data;

	data = kzalloc(sizeof(*data), GFP_KERNEL);
	if (data != NULL) {
		struct nfs_server *server = NFS_SERVER(dir);

		data->msg.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_CREATE];
		data->msg.rpc_argp = &data->arg;
		data->msg.rpc_resp = &data->res;
		data->arg.dir_fh = NFS_FH(dir);
		data->arg.server = server;
		data->arg.name = name;
		data->arg.attrs = sattr;
		data->arg.ftype = ftype;
		data->arg.bitmask = server->attr_bitmask;
		data->res.server = server;
		data->res.fh = &data->fh;
		data->res.fattr = &data->fattr;
		nfs_fattr_init(data->res.fattr);
	}
	return data;
}