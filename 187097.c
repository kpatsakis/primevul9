static int decode_bind_conn_to_session(struct xdr_stream *xdr,
				struct nfs41_bind_conn_to_session_res *res)
{
	__be32 *p;
	int status;

	status = decode_op_hdr(xdr, OP_BIND_CONN_TO_SESSION);
	if (!status)
		status = decode_sessionid(xdr, &res->session->sess_id);
	if (unlikely(status))
		return status;

	/* dir flags, rdma mode bool */
	p = xdr_inline_decode(xdr, 8);
	if (unlikely(!p))
		goto out_overflow;

	res->dir = be32_to_cpup(p++);
	if (res->dir == 0 || res->dir > NFS4_CDFS4_BOTH)
		return -EIO;
	if (be32_to_cpup(p) == 0)
		res->use_conn_in_rdma_mode = false;
	else
		res->use_conn_in_rdma_mode = true;

	return 0;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}