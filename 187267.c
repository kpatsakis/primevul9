static int nfs4_xdr_enc_secinfo_no_name(struct rpc_rqst *req,
					struct xdr_stream *xdr,
					struct nfs41_secinfo_no_name_args *args)
{
	struct compound_hdr hdr = {
		.minorversion = nfs4_xdr_minorversion(&args->seq_args),
	};

	encode_compound_hdr(xdr, req, &hdr);
	encode_sequence(xdr, &args->seq_args, &hdr);
	encode_putrootfh(xdr, &hdr);
	encode_secinfo_no_name(xdr, args, &hdr);
	encode_nops(&hdr);
	return 0;
}