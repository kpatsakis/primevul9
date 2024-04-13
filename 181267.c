nfsd4_verify(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
	     union nfsd4_op_u *u)
{
	__be32 status;

	status = _nfsd4_verify(rqstp, cstate, &u->nverify);
	return status == nfserr_same ? nfs_ok : status;
}