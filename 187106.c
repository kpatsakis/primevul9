static int nfs4_xdr_dec_pathconf(struct rpc_rqst *req, struct xdr_stream *xdr,
				 struct nfs4_pathconf_res *res)
{
	struct compound_hdr hdr;
	int status;

	status = decode_compound_hdr(xdr, &hdr);
	if (!status)
		status = decode_sequence(xdr, &res->seq_res, req);
	if (!status)
		status = decode_putfh(xdr);
	if (!status)
		status = decode_pathconf(xdr, res->pathconf);
	return status;
}