static int nfs4_xdr_dec_free_stateid(struct rpc_rqst *rqstp,
				     struct xdr_stream *xdr,
				     struct nfs41_free_stateid_res *res)
{
	struct compound_hdr hdr;
	int status;

	status = decode_compound_hdr(xdr, &hdr);
	if (status)
		goto out;
	status = decode_sequence(xdr, &res->seq_res, rqstp);
	if (status)
		goto out;
	status = decode_free_stateid(xdr, res);
out:
	return status;
}