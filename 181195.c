nfsd4_offload_status(struct svc_rqst *rqstp,
		     struct nfsd4_compound_state *cstate,
		     union nfsd4_op_u *u)
{
	struct nfsd4_offload_status *os = &u->offload_status;
	__be32 status = 0;
	struct nfsd4_copy *copy;
	struct nfs4_client *clp = cstate->clp;

	copy = find_async_copy(clp, &os->stateid);
	if (copy) {
		os->count = copy->cp_res.wr_bytes_written;
		nfs4_put_copy(copy);
	} else
		status = nfserr_bad_stateid;

	return status;
}