static void nfs4_xdr_enc_server_caps(struct rpc_rqst *req,
				     struct xdr_stream *xdr,
				     struct nfs4_server_caps_arg *args)
{
	struct compound_hdr hdr = {
		.minorversion = nfs4_xdr_minorversion(&args->seq_args),
	};

	encode_compound_hdr(xdr, req, &hdr);
	encode_sequence(xdr, &args->seq_args, &hdr);
	encode_putfh(xdr, args->fhandle, &hdr);
	encode_getattr_one(xdr, FATTR4_WORD0_SUPPORTED_ATTRS|
			   FATTR4_WORD0_FH_EXPIRE_TYPE|
			   FATTR4_WORD0_LINK_SUPPORT|
			   FATTR4_WORD0_SYMLINK_SUPPORT|
			   FATTR4_WORD0_ACLSUPPORT, &hdr);
	encode_nops(&hdr);
}