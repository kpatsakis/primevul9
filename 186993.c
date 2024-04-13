static int nfs4_xdr_dec_access(struct rpc_rqst *rqstp, struct xdr_stream *xdr,
			       struct nfs4_accessres *res)
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
	status = decode_access(xdr, res);
	if (status != 0)
		goto out;
	decode_getfattr(xdr, res->fattr, res->server);
out:
	return status;
}