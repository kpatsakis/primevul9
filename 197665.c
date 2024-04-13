nfsd4_allocate(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
	       struct nfsd4_fallocate *fallocate)
{
	return nfsd4_fallocate(rqstp, cstate, fallocate, 0);
}