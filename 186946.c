static void nfs4_xdr_enc_destroy_session(struct rpc_rqst *req,
					 struct xdr_stream *xdr,
					 struct nfs4_session *session)
{
	struct compound_hdr hdr = {
		.minorversion = session->clp->cl_mvops->minor_version,
	};

	encode_compound_hdr(xdr, req, &hdr);
	encode_destroy_session(xdr, session, &hdr);
	encode_nops(&hdr);
}