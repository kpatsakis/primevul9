static void nfs4_xdr_enc_symlink(struct rpc_rqst *req, struct xdr_stream *xdr,
				 const struct nfs4_create_arg *args)
{
	nfs4_xdr_enc_create(req, xdr, args);
}