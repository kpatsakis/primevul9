static void nfs4_xdr_enc_renew(struct rpc_rqst *req, struct xdr_stream *xdr,
			       struct nfs_client *clp)
{
	struct compound_hdr hdr = {
		.nops	= 0,
	};

	encode_compound_hdr(xdr, req, &hdr);
	encode_renew(xdr, clp->cl_clientid, &hdr);
	encode_nops(&hdr);
}