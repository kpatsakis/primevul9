static void nfs4_xdr_enc_open_confirm(struct rpc_rqst *req,
				      struct xdr_stream *xdr,
				      struct nfs_open_confirmargs *args)
{
	struct compound_hdr hdr = {
		.nops   = 0,
	};

	encode_compound_hdr(xdr, req, &hdr);
	encode_putfh(xdr, args->fh, &hdr);
	encode_open_confirm(xdr, args, &hdr);
	encode_nops(&hdr);
}