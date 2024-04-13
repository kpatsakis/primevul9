nfsd4_allocate(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
	       union nfsd4_op_u *u)
{
	return nfsd4_fallocate(rqstp, cstate, &u->allocate, 0);
}