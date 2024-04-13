static void nfs4_init_channel_attrs(struct nfs41_create_session_args *args)
{
	struct nfs4_session *session = args->client->cl_session;
	unsigned int mxrqst_sz = session->fc_attrs.max_rqst_sz,
		     mxresp_sz = session->fc_attrs.max_resp_sz;

	if (mxrqst_sz == 0)
		mxrqst_sz = NFS_MAX_FILE_IO_SIZE;
	if (mxresp_sz == 0)
		mxresp_sz = NFS_MAX_FILE_IO_SIZE;
	/* Fore channel attributes */
	args->fc_attrs.max_rqst_sz = mxrqst_sz;
	args->fc_attrs.max_resp_sz = mxresp_sz;
	args->fc_attrs.max_ops = NFS4_MAX_OPS;
	args->fc_attrs.max_reqs = max_session_slots;

	dprintk("%s: Fore Channel : max_rqst_sz=%u max_resp_sz=%u "
		"max_ops=%u max_reqs=%u\n",
		__func__,
		args->fc_attrs.max_rqst_sz, args->fc_attrs.max_resp_sz,
		args->fc_attrs.max_ops, args->fc_attrs.max_reqs);

	/* Back channel attributes */
	args->bc_attrs.max_rqst_sz = PAGE_SIZE;
	args->bc_attrs.max_resp_sz = PAGE_SIZE;
	args->bc_attrs.max_resp_sz_cached = 0;
	args->bc_attrs.max_ops = NFS4_MAX_BACK_CHANNEL_OPS;
	args->bc_attrs.max_reqs = 1;

	dprintk("%s: Back Channel : max_rqst_sz=%u max_resp_sz=%u "
		"max_resp_sz_cached=%u max_ops=%u max_reqs=%u\n",
		__func__,
		args->bc_attrs.max_rqst_sz, args->bc_attrs.max_resp_sz,
		args->bc_attrs.max_resp_sz_cached, args->bc_attrs.max_ops,
		args->bc_attrs.max_reqs);
}