nfsd4_copy(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
		union nfsd4_op_u *u)
{
	struct nfsd4_copy *copy = &u->copy;
	__be32 status;
	struct nfsd4_copy *async_copy = NULL;

	status = nfsd4_verify_copy(rqstp, cstate, &copy->cp_src_stateid,
				   &copy->file_src, &copy->cp_dst_stateid,
				   &copy->file_dst);
	if (status)
		goto out;

	copy->cp_clp = cstate->clp;
	memcpy(&copy->fh, &cstate->current_fh.fh_handle,
		sizeof(struct knfsd_fh));
	if (!copy->cp_synchronous) {
		struct nfsd_net *nn = net_generic(SVC_NET(rqstp), nfsd_net_id);

		status = nfserrno(-ENOMEM);
		async_copy = kzalloc(sizeof(struct nfsd4_copy), GFP_KERNEL);
		if (!async_copy)
			goto out;
		if (!nfs4_init_cp_state(nn, copy)) {
			kfree(async_copy);
			goto out;
		}
		refcount_set(&async_copy->refcount, 1);
		memcpy(&copy->cp_res.cb_stateid, &copy->cp_stateid,
			sizeof(copy->cp_stateid));
		dup_copy_fields(copy, async_copy);
		async_copy->copy_task = kthread_create(nfsd4_do_async_copy,
				async_copy, "%s", "copy thread");
		if (IS_ERR(async_copy->copy_task))
			goto out_err;
		spin_lock(&async_copy->cp_clp->async_lock);
		list_add(&async_copy->copies,
				&async_copy->cp_clp->async_copies);
		spin_unlock(&async_copy->cp_clp->async_lock);
		wake_up_process(async_copy->copy_task);
		status = nfs_ok;
	} else
		status = nfsd4_do_copy(copy, 1);
out:
	return status;
out_err:
	cleanup_async_copy(async_copy);
	goto out;
}