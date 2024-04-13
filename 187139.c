static void nfs4_xdr_enc_setacl(struct rpc_rqst *req, struct xdr_stream *xdr,
				struct nfs_setaclargs *args)
{
	struct compound_hdr hdr = {
		.minorversion = nfs4_xdr_minorversion(&args->seq_args),
	};

	encode_compound_hdr(xdr, req, &hdr);
	encode_sequence(xdr, &args->seq_args, &hdr);
	encode_putfh(xdr, args->fh, &hdr);
	encode_setacl(xdr, args, &hdr);
	encode_nops(&hdr);
}