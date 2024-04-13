static void nfs4_xdr_enc_create_session(struct rpc_rqst *req,
					struct xdr_stream *xdr,
					struct nfs41_create_session_args *args)
{
	struct compound_hdr hdr = {
		.minorversion = args->client->cl_mvops->minor_version,
	};

	encode_compound_hdr(xdr, req, &hdr);
	encode_create_session(xdr, args, &hdr);
	encode_nops(&hdr);
}