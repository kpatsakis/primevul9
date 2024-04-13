static int _nfs4_do_open(struct inode *dir,
			struct dentry *dentry,
			fmode_t fmode,
			int flags,
			struct iattr *sattr,
			struct rpc_cred *cred,
			struct nfs4_state **res,
			struct nfs4_threshold **ctx_th)
{
	struct nfs4_state_owner  *sp;
	struct nfs4_state     *state = NULL;
	struct nfs_server       *server = NFS_SERVER(dir);
	struct nfs4_opendata *opendata;
	int status;

	/* Protect against reboot recovery conflicts */
	status = -ENOMEM;
	sp = nfs4_get_state_owner(server, cred, GFP_KERNEL);
	if (sp == NULL) {
		dprintk("nfs4_do_open: nfs4_get_state_owner failed!\n");
		goto out_err;
	}
	status = nfs4_recover_expired_lease(server);
	if (status != 0)
		goto err_put_state_owner;
	if (dentry->d_inode != NULL)
		nfs4_return_incompatible_delegation(dentry->d_inode, fmode);
	status = -ENOMEM;
	opendata = nfs4_opendata_alloc(dentry, sp, fmode, flags, sattr, GFP_KERNEL);
	if (opendata == NULL)
		goto err_put_state_owner;

	if (ctx_th && server->attr_bitmask[2] & FATTR4_WORD2_MDSTHRESHOLD) {
		opendata->f_attr.mdsthreshold = pnfs_mdsthreshold_alloc();
		if (!opendata->f_attr.mdsthreshold)
			goto err_opendata_put;
		opendata->o_arg.open_bitmap = &nfs4_pnfs_open_bitmap[0];
	}
	if (dentry->d_inode != NULL)
		opendata->state = nfs4_get_open_state(dentry->d_inode, sp);

	status = _nfs4_proc_open(opendata);
	if (status != 0)
		goto err_opendata_put;

	state = nfs4_opendata_to_nfs4_state(opendata);
	status = PTR_ERR(state);
	if (IS_ERR(state))
		goto err_opendata_put;
	if (server->caps & NFS_CAP_POSIX_LOCK)
		set_bit(NFS_STATE_POSIX_LOCKS, &state->flags);

	if (opendata->o_arg.open_flags & O_EXCL) {
		nfs4_exclusive_attrset(opendata, sattr);

		nfs_fattr_init(opendata->o_res.f_attr);
		status = nfs4_do_setattr(state->inode, cred,
				opendata->o_res.f_attr, sattr,
				state);
		if (status == 0)
			nfs_setattr_update_inode(state->inode, sattr);
		nfs_post_op_update_inode(state->inode, opendata->o_res.f_attr);
	}

	if (pnfs_use_threshold(ctx_th, opendata->f_attr.mdsthreshold, server))
		*ctx_th = opendata->f_attr.mdsthreshold;
	else
		kfree(opendata->f_attr.mdsthreshold);
	opendata->f_attr.mdsthreshold = NULL;

	nfs4_opendata_put(opendata);
	nfs4_put_state_owner(sp);
	*res = state;
	return 0;
err_opendata_put:
	kfree(opendata->f_attr.mdsthreshold);
	nfs4_opendata_put(opendata);
err_put_state_owner:
	nfs4_put_state_owner(sp);
out_err:
	*res = NULL;
	return status;
}