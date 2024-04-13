static void nfs4_xdr_enc_layoutreturn(struct rpc_rqst *req,
				      struct xdr_stream *xdr,
				      struct nfs4_layoutreturn_args *args)
{
	struct compound_hdr hdr = {
		.minorversion = nfs4_xdr_minorversion(&args->seq_args),
	};

	encode_compound_hdr(xdr, req, &hdr);
	encode_sequence(xdr, &args->seq_args, &hdr);
	encode_putfh(xdr, NFS_FH(args->inode), &hdr);
	encode_layoutreturn(xdr, args, &hdr);
	encode_nops(&hdr);
}