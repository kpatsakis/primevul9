int nfs4_proc_exchange_id(struct nfs_client *clp, struct rpc_cred *cred)
{
	nfs4_verifier verifier;
	struct nfs41_exchange_id_args args = {
		.verifier = &verifier,
		.client = clp,
		.flags = EXCHGID4_FLAG_SUPP_MOVED_REFER,
	};
	struct nfs41_exchange_id_res res = {
		0
	};
	int status;
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_EXCHANGE_ID],
		.rpc_argp = &args,
		.rpc_resp = &res,
		.rpc_cred = cred,
	};

	nfs4_init_boot_verifier(clp, &verifier);
	args.id_len = scnprintf(args.id, sizeof(args.id),
				"%s/%s",
				clp->cl_ipaddr,
				clp->cl_rpcclient->cl_nodename);
	dprintk("NFS call  exchange_id auth=%s, '%.*s'\n",
		clp->cl_rpcclient->cl_auth->au_ops->au_name,
		args.id_len, args.id);

	res.server_owner = kzalloc(sizeof(struct nfs41_server_owner),
					GFP_NOFS);
	if (unlikely(res.server_owner == NULL)) {
		status = -ENOMEM;
		goto out;
	}

	res.server_scope = kzalloc(sizeof(struct nfs41_server_scope),
					GFP_NOFS);
	if (unlikely(res.server_scope == NULL)) {
		status = -ENOMEM;
		goto out_server_owner;
	}

	res.impl_id = kzalloc(sizeof(struct nfs41_impl_id), GFP_NOFS);
	if (unlikely(res.impl_id == NULL)) {
		status = -ENOMEM;
		goto out_server_scope;
	}

	status = rpc_call_sync(clp->cl_rpcclient, &msg, RPC_TASK_TIMEOUT);
	if (status == 0)
		status = nfs4_check_cl_exchange_flags(res.flags);

	if (status == 0) {
		clp->cl_clientid = res.clientid;
		clp->cl_exchange_flags = (res.flags & ~EXCHGID4_FLAG_CONFIRMED_R);
		if (!(res.flags & EXCHGID4_FLAG_CONFIRMED_R))
			clp->cl_seqid = res.seqid;

		kfree(clp->cl_serverowner);
		clp->cl_serverowner = res.server_owner;
		res.server_owner = NULL;

		/* use the most recent implementation id */
		kfree(clp->cl_implid);
		clp->cl_implid = res.impl_id;

		if (clp->cl_serverscope != NULL &&
		    !nfs41_same_server_scope(clp->cl_serverscope,
					     res.server_scope)) {
			dprintk("%s: server_scope mismatch detected\n",
				__func__);
			set_bit(NFS4CLNT_SERVER_SCOPE_MISMATCH, &clp->cl_state);
			kfree(clp->cl_serverscope);
			clp->cl_serverscope = NULL;
		}

		if (clp->cl_serverscope == NULL) {
			clp->cl_serverscope = res.server_scope;
			goto out;
		}
	} else
		kfree(res.impl_id);

out_server_owner:
	kfree(res.server_owner);
out_server_scope:
	kfree(res.server_scope);
out:
	if (clp->cl_implid != NULL)
		dprintk("NFS reply exchange_id: Server Implementation ID: "
			"domain: %s, name: %s, date: %llu,%u\n",
			clp->cl_implid->domain, clp->cl_implid->name,
			clp->cl_implid->date.seconds,
			clp->cl_implid->date.nseconds);
	dprintk("NFS reply exchange_id: %d\n", status);
	return status;
}