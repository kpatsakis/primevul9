nfsd4_nverify(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate,
	      struct nfsd4_verify *verify)
{
	__be32 status;

	status = _nfsd4_verify(rqstp, cstate, verify);
	return status == nfserr_not_same ? nfs_ok : status;
}