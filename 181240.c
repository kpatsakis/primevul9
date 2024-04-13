nfsd4_readlink(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
	       union nfsd4_op_u *u)
{
	u->readlink.rl_rqstp = rqstp;
	u->readlink.rl_fhp = &cstate->current_fh;
	return nfs_ok;
}