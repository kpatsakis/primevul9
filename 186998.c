static void nfs4_xdr_enc_link(struct rpc_rqst *req, struct xdr_stream *xdr,
			     const struct nfs4_link_arg *args)
{
	struct compound_hdr hdr = {
		.minorversion = nfs4_xdr_minorversion(&args->seq_args),
	};

	encode_compound_hdr(xdr, req, &hdr);
	encode_sequence(xdr, &args->seq_args, &hdr);
	encode_putfh(xdr, args->fh, &hdr);
	encode_savefh(xdr, &hdr);
	encode_putfh(xdr, args->dir_fh, &hdr);
	encode_link(xdr, args->name, &hdr);
	encode_restorefh(xdr, &hdr);
	encode_getfattr(xdr, args->bitmask, &hdr);
	encode_nops(&hdr);
}