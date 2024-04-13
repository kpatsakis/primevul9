static void encode_bind_conn_to_session(struct xdr_stream *xdr,
				   struct nfs4_session *session,
				   struct compound_hdr *hdr)
{
	__be32 *p;

	encode_op_hdr(xdr, OP_BIND_CONN_TO_SESSION,
		decode_bind_conn_to_session_maxsz, hdr);
	encode_opaque_fixed(xdr, session->sess_id.data, NFS4_MAX_SESSIONID_LEN);
	p = xdr_reserve_space(xdr, 8);
	*p++ = cpu_to_be32(NFS4_CDFC4_BACK_OR_BOTH);
	*p = 0;	/* use_conn_in_rdma_mode = False */
}