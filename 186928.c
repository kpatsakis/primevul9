static int nfs4_proc_fsinfo(struct nfs_server *server, struct nfs_fh *fhandle, struct nfs_fsinfo *fsinfo)
{
	int error;

	nfs_fattr_init(fsinfo->fattr);
	error = nfs4_do_fsinfo(server, fhandle, fsinfo);
	if (error == 0)
		set_pnfs_layoutdriver(server, fhandle, fsinfo->layouttype);

	return error;
}