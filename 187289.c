static void nfs4_xdr_enc_destroy_clientid(struct rpc_rqst *req,
					 struct xdr_stream *xdr,
					 struct nfs_client *clp)
{
	struct compound_hdr hdr = {
		.minorversion = clp->cl_mvops->minor_version,
	};

	encode_compound_hdr(xdr, req, &hdr);
	encode_destroy_clientid(xdr, clp->cl_clientid, &hdr);
	encode_nops(&hdr);
}