static void encode_destroy_session(struct xdr_stream *xdr,
				   struct nfs4_session *session,
				   struct compound_hdr *hdr)
{
	encode_op_hdr(xdr, OP_DESTROY_SESSION, decode_destroy_session_maxsz, hdr);
	encode_opaque_fixed(xdr, session->sess_id.data, NFS4_MAX_SESSIONID_LEN);
}