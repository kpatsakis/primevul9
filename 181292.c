nfsd4_deallocate(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
		 union nfsd4_op_u *u)
{
	return nfsd4_fallocate(rqstp, cstate, &u->deallocate,
			       FALLOC_FL_PUNCH_HOLE | FALLOC_FL_KEEP_SIZE);
}