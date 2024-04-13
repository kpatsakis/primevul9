static void nfs4_xdr_enc_sequence(struct rpc_rqst *req, struct xdr_stream *xdr,
				  struct nfs4_sequence_args *args)
{
	struct compound_hdr hdr = {
		.minorversion = nfs4_xdr_minorversion(args),
	};

	encode_compound_hdr(xdr, req, &hdr);
	encode_sequence(xdr, args, &hdr);
	encode_nops(&hdr);
}