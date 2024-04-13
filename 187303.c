static int nfs4_xdr_dec_delegreturn(struct rpc_rqst *rqstp,
				    struct xdr_stream *xdr,
				    struct nfs4_delegreturnres *res)
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
	if (status != 0)
		goto out;
	status = decode_getfattr(xdr, res->fattr, res->server);
	if (status != 0)
		goto out;
	status = decode_delegreturn(xdr);
out:
	return status;
}