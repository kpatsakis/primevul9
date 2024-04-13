static int nfs4_xdr_dec_getdevicelist(struct rpc_rqst *rqstp,
				      struct xdr_stream *xdr,
				      struct nfs4_getdevicelist_res *res)
{
	struct compound_hdr hdr;
	int status;

	dprintk("encoding getdevicelist!\n");

	status = decode_compound_hdr(xdr, &hdr);
	if (status != 0)
		goto out;
	status = decode_sequence(xdr, &res->seq_res, rqstp);
	if (status != 0)
		goto out;
	status = decode_putfh(xdr);
	if (status != 0)
		goto out;
	status = decode_getdevicelist(xdr, res->devlist);
out:
	return status;
}