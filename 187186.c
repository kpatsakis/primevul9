static int nfs4_xdr_dec_open(struct rpc_rqst *rqstp, struct xdr_stream *xdr,
			     struct nfs_openres *res)
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
	status = decode_open(xdr, res);
	if (status)
		goto out;
	if (decode_getfh(xdr, &res->fh) != 0)
		goto out;
	decode_getfattr(xdr, res->f_attr, res->server);
out:
	return status;
}