static void nfs4_xdr_enc_release_lockowner(struct rpc_rqst *req,
					   struct xdr_stream *xdr,
					struct nfs_release_lockowner_args *args)
{
	struct compound_hdr hdr = {
		.minorversion = 0,
	};

	encode_compound_hdr(xdr, req, &hdr);
	encode_release_lockowner(xdr, &args->lock_owner, &hdr);
	encode_nops(&hdr);
}