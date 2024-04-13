static void nfs4_xdr_enc_free_stateid(struct rpc_rqst *req,
				     struct xdr_stream *xdr,
				     struct nfs41_free_stateid_args *args)
{
	struct compound_hdr hdr = {
		.minorversion = nfs4_xdr_minorversion(&args->seq_args),
	};

	encode_compound_hdr(xdr, req, &hdr);
	encode_sequence(xdr, &args->seq_args, &hdr);
	encode_free_stateid(xdr, args, &hdr);
	encode_nops(&hdr);
}