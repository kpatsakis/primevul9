static int nfs4_xdr_dec_write(struct rpc_rqst *rqstp, struct xdr_stream *xdr,
			      struct nfs_writeres *res)
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
	status = decode_write(xdr, res);
	if (status)
		goto out;
	if (res->fattr)
		decode_getfattr(xdr, res->fattr, res->server);
	if (!status)
		status = res->count;
out:
	return status;
}