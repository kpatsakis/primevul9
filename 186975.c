static int nfs4_xdr_dec_setclientid(struct rpc_rqst *req,
				    struct xdr_stream *xdr,
				    struct nfs4_setclientid_res *res)
{
	struct compound_hdr hdr;
	int status;

	status = decode_compound_hdr(xdr, &hdr);
	if (!status)
		status = decode_setclientid(xdr, res);
	return status;
}