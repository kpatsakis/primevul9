static void nfs4_xdr_enc_bind_conn_to_session(struct rpc_rqst *req,
				struct xdr_stream *xdr,
				struct nfs_client *clp)
{
	struct compound_hdr hdr = {
		.minorversion = clp->cl_mvops->minor_version,
	};

	encode_compound_hdr(xdr, req, &hdr);
	encode_bind_conn_to_session(xdr, clp->cl_session, &hdr);
	encode_nops(&hdr);
}