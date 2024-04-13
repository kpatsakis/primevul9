static int decode_exchange_id(struct xdr_stream *xdr,
			      struct nfs41_exchange_id_res *res)
{
	__be32 *p;
	uint32_t dummy;
	char *dummy_str;
	int status;
	uint32_t impl_id_count;

	status = decode_op_hdr(xdr, OP_EXCHANGE_ID);
	if (status)
		return status;

	p = xdr_inline_decode(xdr, 8);
	if (unlikely(!p))
		goto out_overflow;
	xdr_decode_hyper(p, &res->clientid);
	p = xdr_inline_decode(xdr, 12);
	if (unlikely(!p))
		goto out_overflow;
	res->seqid = be32_to_cpup(p++);
	res->flags = be32_to_cpup(p++);

	/* We ask for SP4_NONE */
	dummy = be32_to_cpup(p);
	if (dummy != SP4_NONE)
		return -EIO;

	/* server_owner4.so_minor_id */
	p = xdr_inline_decode(xdr, 8);
	if (unlikely(!p))
		goto out_overflow;
	p = xdr_decode_hyper(p, &res->server_owner->minor_id);

	/* server_owner4.so_major_id */
	status = decode_opaque_inline(xdr, &dummy, &dummy_str);
	if (unlikely(status))
		return status;
	if (unlikely(dummy > NFS4_OPAQUE_LIMIT))
		return -EIO;
	memcpy(res->server_owner->major_id, dummy_str, dummy);
	res->server_owner->major_id_sz = dummy;

	/* server_scope4 */
	status = decode_opaque_inline(xdr, &dummy, &dummy_str);
	if (unlikely(status))
		return status;
	if (unlikely(dummy > NFS4_OPAQUE_LIMIT))
		return -EIO;
	memcpy(res->server_scope->server_scope, dummy_str, dummy);
	res->server_scope->server_scope_sz = dummy;

	/* Implementation Id */
	p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p))
		goto out_overflow;
	impl_id_count = be32_to_cpup(p++);

	if (impl_id_count) {
		/* nii_domain */
		status = decode_opaque_inline(xdr, &dummy, &dummy_str);
		if (unlikely(status))
			return status;
		if (unlikely(dummy > NFS4_OPAQUE_LIMIT))
			return -EIO;
		memcpy(res->impl_id->domain, dummy_str, dummy);

		/* nii_name */
		status = decode_opaque_inline(xdr, &dummy, &dummy_str);
		if (unlikely(status))
			return status;
		if (unlikely(dummy > NFS4_OPAQUE_LIMIT))
			return -EIO;
		memcpy(res->impl_id->name, dummy_str, dummy);

		/* nii_date */
		p = xdr_inline_decode(xdr, 12);
		if (unlikely(!p))
			goto out_overflow;
		p = xdr_decode_hyper(p, &res->impl_id->date.seconds);
		res->impl_id->date.nseconds = be32_to_cpup(p);

		/* if there's more than one entry, ignore the rest */
	}
	return 0;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}