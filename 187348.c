static int nfs4_do_create(struct inode *dir, struct dentry *dentry, struct nfs4_createdata *data)
{
	int status = nfs4_call_sync(NFS_SERVER(dir)->client, NFS_SERVER(dir), &data->msg,
				    &data->arg.seq_args, &data->res.seq_res, 1);
	if (status == 0) {
		update_changeattr(dir, &data->res.dir_cinfo);
		status = nfs_instantiate(dentry, data->res.fh, data->res.fattr);
	}
	return status;
}