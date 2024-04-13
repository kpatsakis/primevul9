static int nfs4_xdr_dec_locku(struct rpc_rqst *rqstp, struct xdr_stream *xdr,
			      struct nfs_locku_res *res)
{
	struct compound_hdr hdr;
	int status;

	status = decode_compound_hdr(xdr, &hdr);
	if (status)
		goto out;
	status = decode_sequence(xdr, &res->seq_res, rqstp);
	if (status)
		goto out;
	status = decode_putfh(xdr);
	if (status)
		goto out;
	status = decode_locku(xdr, res);
out:
	return status;
}