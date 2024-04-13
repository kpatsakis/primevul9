static int nfs4_xdr_dec_close(struct rpc_rqst *rqstp, struct xdr_stream *xdr,
			      struct nfs_closeres *res)
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
	status = decode_close(xdr, res);
	if (status != 0)
		goto out;
	/*
	 * Note: Server may do delete on close for this file
	 * 	in which case the getattr call will fail with
	 * 	an ESTALE error. Shouldn't be a problem,
	 * 	though, since fattr->valid will remain unset.
	 */
	decode_getfattr(xdr, res->fattr, res->server);
out:
	return status;
}