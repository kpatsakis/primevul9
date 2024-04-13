static int decode_create_session(struct xdr_stream *xdr,
				 struct nfs41_create_session_res *res)
{
	__be32 *p;
	int status;
	struct nfs_client *clp = res->client;
	struct nfs4_session *session = clp->cl_session;

	status = decode_op_hdr(xdr, OP_CREATE_SESSION);
	if (!status)
		status = decode_sessionid(xdr, &session->sess_id);
	if (unlikely(status))
		return status;

	/* seqid, flags */
	p = xdr_inline_decode(xdr, 8);
	if (unlikely(!p))
		goto out_overflow;
	clp->cl_seqid = be32_to_cpup(p++);
	session->flags = be32_to_cpup(p);

	/* Channel attributes */
	status = decode_chan_attrs(xdr, &session->fc_attrs);
	if (!status)
		status = decode_chan_attrs(xdr, &session->bc_attrs);
	return status;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}