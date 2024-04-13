static int nfs4_xdr_dec_setclientid_confirm(struct rpc_rqst *req,
					    struct xdr_stream *xdr,
					    struct nfs_fsinfo *fsinfo)
{
	struct compound_hdr hdr;
	int status;

	status = decode_compound_hdr(xdr, &hdr);
	if (!status)
		status = decode_setclientid_confirm(xdr);
	if (!status)
		status = decode_putrootfh(xdr);
	if (!status)
		status = decode_fsinfo(xdr, fsinfo);
	return status;
}