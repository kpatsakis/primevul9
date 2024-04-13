static int nfs4_xdr_dec_sequence(struct rpc_rqst *rqstp,
				 struct xdr_stream *xdr,
				 struct nfs4_sequence_res *res)
{
	struct compound_hdr hdr;
	int status;

	status = decode_compound_hdr(xdr, &hdr);
	if (!status)
		status = decode_sequence(xdr, res, rqstp);
	return status;
}