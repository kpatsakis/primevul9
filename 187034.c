static void nfs4_xdr_enc_exchange_id(struct rpc_rqst *req,
				     struct xdr_stream *xdr,
				     struct nfs41_exchange_id_args *args)
{
	struct compound_hdr hdr = {
		.minorversion = args->client->cl_mvops->minor_version,
	};

	encode_compound_hdr(xdr, req, &hdr);
	encode_exchange_id(xdr, args, &hdr);
	encode_nops(&hdr);
}