static void nfs4_xdr_enc_layoutcommit(struct rpc_rqst *req,
				      struct xdr_stream *xdr,
				      struct nfs4_layoutcommit_args *args)
{
	struct nfs4_layoutcommit_data *data =
		container_of(args, struct nfs4_layoutcommit_data, args);
	struct compound_hdr hdr = {
		.minorversion = nfs4_xdr_minorversion(&args->seq_args),
	};

	encode_compound_hdr(xdr, req, &hdr);
	encode_sequence(xdr, &args->seq_args, &hdr);
	encode_putfh(xdr, NFS_FH(args->inode), &hdr);
	encode_layoutcommit(xdr, data->args.inode, args, &hdr);
	encode_getfattr(xdr, args->bitmask, &hdr);
	encode_nops(&hdr);
}