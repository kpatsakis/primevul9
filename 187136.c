static void nfs4_xdr_enc_pathconf(struct rpc_rqst *req, struct xdr_stream *xdr,
				  const struct nfs4_pathconf_arg *args)
{
	struct compound_hdr hdr = {
		.minorversion = nfs4_xdr_minorversion(&args->seq_args),
	};

	encode_compound_hdr(xdr, req, &hdr);
	encode_sequence(xdr, &args->seq_args, &hdr);
	encode_putfh(xdr, args->fh, &hdr);
	encode_getattr_one(xdr, args->bitmask[0] & nfs4_pathconf_bitmap[0],
			   &hdr);
	encode_nops(&hdr);
}