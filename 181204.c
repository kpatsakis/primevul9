nfsd4_getfh(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
	    union nfsd4_op_u *u)
{
	u->getfh = &cstate->current_fh;
	return nfs_ok;
}