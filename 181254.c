nfsd4_lookup(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
	     union nfsd4_op_u *u)
{
	return nfsd_lookup(rqstp, &cstate->current_fh,
			   u->lookup.lo_name, u->lookup.lo_len,
			   &cstate->current_fh);
}