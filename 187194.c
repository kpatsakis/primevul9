static void nfs4_xdr_enc_rename(struct rpc_rqst *req, struct xdr_stream *xdr,
				const struct nfs_renameargs *args)
{
	struct compound_hdr hdr = {
		.minorversion = nfs4_xdr_minorversion(&args->seq_args),
	};

	encode_compound_hdr(xdr, req, &hdr);
	encode_sequence(xdr, &args->seq_args, &hdr);
	encode_putfh(xdr, args->old_dir, &hdr);
	encode_savefh(xdr, &hdr);
	encode_putfh(xdr, args->new_dir, &hdr);
	encode_rename(xdr, args->old_name, args->new_name, &hdr);
	encode_nops(&hdr);
}