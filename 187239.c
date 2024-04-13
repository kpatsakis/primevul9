static int _nfs4_proc_open(struct nfs4_opendata *data)
{
	struct inode *dir = data->dir->d_inode;
	struct nfs_server *server = NFS_SERVER(dir);
	struct nfs_openargs *o_arg = &data->o_arg;
	struct nfs_openres *o_res = &data->o_res;
	int status;

	status = nfs4_run_open_task(data, 0);
	if (!data->rpc_done)
		return status;
	if (status != 0) {
		if (status == -NFS4ERR_BADNAME &&
				!(o_arg->open_flags & O_CREAT))
			return -ENOENT;
		return status;
	}

	nfs_fattr_map_and_free_names(server, &data->f_attr);

	if (o_arg->open_flags & O_CREAT)
		update_changeattr(dir, &o_res->cinfo);
	if ((o_res->rflags & NFS4_OPEN_RESULT_LOCKTYPE_POSIX) == 0)
		server->caps &= ~NFS_CAP_POSIX_LOCK;
	if(o_res->rflags & NFS4_OPEN_RESULT_CONFIRM) {
		status = _nfs4_proc_open_confirm(data);
		if (status != 0)
			return status;
	}
	if (!(o_res->f_attr->valid & NFS_ATTR_FATTR))
		_nfs4_proc_getattr(server, &o_res->fh, o_res->f_attr);
	return 0;
}