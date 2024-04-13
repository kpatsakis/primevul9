nfsd4_nverify(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
	      union nfsd4_op_u *u)
{
	__be32 status;

	status = _nfsd4_verify(rqstp, cstate, &u->verify);
	return status == nfserr_not_same ? nfs_ok : status;
}