nfs41_find_root_sec(struct nfs_server *server, struct nfs_fh *fhandle,
		    struct nfs_fsinfo *info)
{
	int err;
	struct page *page;
	rpc_authflavor_t flavor;
	struct nfs4_secinfo_flavors *flavors;

	page = alloc_page(GFP_KERNEL);
	if (!page) {
		err = -ENOMEM;
		goto out;
	}

	flavors = page_address(page);
	err = nfs41_proc_secinfo_no_name(server, fhandle, info, flavors);

	/*
	 * Fall back on "guess and check" method if
	 * the server doesn't support SECINFO_NO_NAME
	 */
	if (err == -NFS4ERR_WRONGSEC || err == -NFS4ERR_NOTSUPP) {
		err = nfs4_find_root_sec(server, fhandle, info);
		goto out_freepage;
	}
	if (err)
		goto out_freepage;

	flavor = nfs_find_best_sec(flavors);
	if (err == 0)
		err = nfs4_lookup_root_sec(server, fhandle, info, flavor);

out_freepage:
	put_page(page);
	if (err == -EACCES)
		return -EPERM;
out:
	return err;
}