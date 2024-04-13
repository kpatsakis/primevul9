static void nfs4_xdr_enc_setclientid(struct rpc_rqst *req,
				     struct xdr_stream *xdr,
				     struct nfs4_setclientid *sc)
{
	struct compound_hdr hdr = {
		.nops	= 0,
	};

	encode_compound_hdr(xdr, req, &hdr);
	encode_setclientid(xdr, sc, &hdr);
	encode_nops(&hdr);
}