int nfs4_proc_bind_conn_to_session(struct nfs_client *clp, struct rpc_cred *cred)
{
	int status;
	struct nfs41_bind_conn_to_session_res res;
	struct rpc_message msg = {
		.rpc_proc =
			&nfs4_procedures[NFSPROC4_CLNT_BIND_CONN_TO_SESSION],
		.rpc_argp = clp,
		.rpc_resp = &res,
		.rpc_cred = cred,
	};

	dprintk("--> %s\n", __func__);
	BUG_ON(clp == NULL);

	res.session = kzalloc(sizeof(struct nfs4_session), GFP_NOFS);
	if (unlikely(res.session == NULL)) {
		status = -ENOMEM;
		goto out;
	}

	status = rpc_call_sync(clp->cl_rpcclient, &msg, RPC_TASK_TIMEOUT);
	if (status == 0) {
		if (memcmp(res.session->sess_id.data,
		    clp->cl_session->sess_id.data, NFS4_MAX_SESSIONID_LEN)) {
			dprintk("NFS: %s: Session ID mismatch\n", __func__);
			status = -EIO;
			goto out_session;
		}
		if (res.dir != NFS4_CDFS4_BOTH) {
			dprintk("NFS: %s: Unexpected direction from server\n",
				__func__);
			status = -EIO;
			goto out_session;
		}
		if (res.use_conn_in_rdma_mode) {
			dprintk("NFS: %s: Server returned RDMA mode = true\n",
				__func__);
			status = -EIO;
			goto out_session;
		}
	}
out_session:
	kfree(res.session);
out:
	dprintk("<-- %s status= %d\n", __func__, status);
	return status;
}