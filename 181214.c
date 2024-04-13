nfsd4_access(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
	     union nfsd4_op_u *u)
{
	struct nfsd4_access *access = &u->access;

	if (access->ac_req_access & ~NFS3_ACCESS_FULL)
		return nfserr_inval;

	access->ac_resp_access = access->ac_req_access;
	return nfsd_access(rqstp, &cstate->current_fh, &access->ac_resp_access,
			   &access->ac_supported);
}