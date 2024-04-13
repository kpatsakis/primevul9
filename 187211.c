static int nfs4_xdr_dec_read(struct rpc_rqst *rqstp, struct xdr_stream *xdr,
			     struct nfs_readres *res)
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
	status = decode_read(xdr, rqstp, res);
	if (!status)
		status = res->count;
out:
	return status;
}