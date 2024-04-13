nfsd4_lookupp(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
	      void *arg)
{
	return nfsd4_do_lookupp(rqstp, &cstate->current_fh);
}