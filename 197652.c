static __be32 nfsd4_open_omfg(struct svc_rqst *rqstp, struct nfsd4_compound_state *cstate, struct nfsd4_op *op)
{
	struct nfsd4_open *open = (struct nfsd4_open *)&op->u;

	if (!seqid_mutating_err(ntohl(op->status)))
		return op->status;
	if (nfsd4_has_session(cstate))
		return op->status;
	open->op_xdr_error = op->status;
	return nfsd4_open(rqstp, cstate, open);
}