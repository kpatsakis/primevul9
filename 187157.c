int nfs4_proc_setclientid(struct nfs_client *clp, u32 program,
		unsigned short port, struct rpc_cred *cred,
		struct nfs4_setclientid_res *res)
{
	nfs4_verifier sc_verifier;
	struct nfs4_setclientid setclientid = {
		.sc_verifier = &sc_verifier,
		.sc_prog = program,
		.sc_cb_ident = clp->cl_cb_ident,
	};
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_SETCLIENTID],
		.rpc_argp = &setclientid,
		.rpc_resp = res,
		.rpc_cred = cred,
	};
	int status;

	/* nfs_client_id4 */
	nfs4_init_boot_verifier(clp, &sc_verifier);
	rcu_read_lock();
	setclientid.sc_name_len = scnprintf(setclientid.sc_name,
			sizeof(setclientid.sc_name), "%s/%s %s",
			clp->cl_ipaddr,
			rpc_peeraddr2str(clp->cl_rpcclient,
						RPC_DISPLAY_ADDR),
			rpc_peeraddr2str(clp->cl_rpcclient,
						RPC_DISPLAY_PROTO));
	/* cb_client4 */
	setclientid.sc_netid_len = scnprintf(setclientid.sc_netid,
				sizeof(setclientid.sc_netid),
				rpc_peeraddr2str(clp->cl_rpcclient,
							RPC_DISPLAY_NETID));
	rcu_read_unlock();
	setclientid.sc_uaddr_len = scnprintf(setclientid.sc_uaddr,
				sizeof(setclientid.sc_uaddr), "%s.%u.%u",
				clp->cl_ipaddr, port >> 8, port & 255);

	dprintk("NFS call  setclientid auth=%s, '%.*s'\n",
		clp->cl_rpcclient->cl_auth->au_ops->au_name,
		setclientid.sc_name_len, setclientid.sc_name);
	status = rpc_call_sync(clp->cl_rpcclient, &msg, RPC_TASK_TIMEOUT);
	dprintk("NFS reply setclientid: %d\n", status);
	return status;
}