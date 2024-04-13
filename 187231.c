static int _nfs4_proc_symlink(struct inode *dir, struct dentry *dentry,
		struct page *page, unsigned int len, struct iattr *sattr)
{
	struct nfs4_createdata *data;
	int status = -ENAMETOOLONG;

	if (len > NFS4_MAXPATHLEN)
		goto out;

	status = -ENOMEM;
	data = nfs4_alloc_createdata(dir, &dentry->d_name, sattr, NF4LNK);
	if (data == NULL)
		goto out;

	data->msg.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_SYMLINK];
	data->arg.u.symlink.pages = &page;
	data->arg.u.symlink.len = len;
	
	status = nfs4_do_create(dir, dentry, data);

	nfs4_free_createdata(data);
out:
	return status;
}