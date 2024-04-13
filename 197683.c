nfsd4_deallocate(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
		 struct nfsd4_fallocate *fallocate)
{
	return nfsd4_fallocate(rqstp, cstate, fallocate,
			       FALLOC_FL_PUNCH_HOLE | FALLOC_FL_KEEP_SIZE);
}