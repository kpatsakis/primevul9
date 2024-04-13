static int nfs4_xdr_dec_open_confirm(struct rpc_rqst *rqstp,
				     struct xdr_stream *xdr,
				     struct nfs_open_confirmres *res)
{
	struct compound_hdr hdr;
	int status;

	status = decode_compound_hdr(xdr, &hdr);
	if (status)
		goto out;
	status = decode_putfh(xdr);
	if (status)
		goto out;
	status = decode_open_confirm(xdr, res);
out:
	return status;
}