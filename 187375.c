static int nfs4_xdr_dec_reclaim_complete(struct rpc_rqst *rqstp,
					 struct xdr_stream *xdr,
					 struct nfs41_reclaim_complete_res *res)
{
	struct compound_hdr hdr;
	int status;

	status = decode_compound_hdr(xdr, &hdr);
	if (!status)
		status = decode_sequence(xdr, &res->seq_res, rqstp);
	if (!status)
		status = decode_reclaim_complete(xdr, (void *)NULL);
	return status;
}