static void nfs4_layoutget_release(void *calldata)
{
	struct nfs4_layoutget *lgp = calldata;
	struct nfs_server *server = NFS_SERVER(lgp->args.inode);
	size_t max_pages = max_response_pages(server);

	dprintk("--> %s\n", __func__);
	nfs4_free_pages(lgp->args.layout.pages, max_pages);
	put_nfs_open_context(lgp->args.ctx);
	kfree(calldata);
	dprintk("<-- %s\n", __func__);
}