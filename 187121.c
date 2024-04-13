static int decode_delegation(struct xdr_stream *xdr, struct nfs_openres *res)
{
	__be32 *p;
	uint32_t delegation_type;
	int status;

	p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p))
		goto out_overflow;
	delegation_type = be32_to_cpup(p);
	if (delegation_type == NFS4_OPEN_DELEGATE_NONE) {
		res->delegation_type = 0;
		return 0;
	}
	status = decode_stateid(xdr, &res->delegation);
	if (unlikely(status))
		return status;
	p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p))
		goto out_overflow;
	res->do_recall = be32_to_cpup(p);

	switch (delegation_type) {
	case NFS4_OPEN_DELEGATE_READ:
		res->delegation_type = FMODE_READ;
		break;
	case NFS4_OPEN_DELEGATE_WRITE:
		res->delegation_type = FMODE_WRITE|FMODE_READ;
		if (decode_space_limit(xdr, &res->maxsize) < 0)
				return -EIO;
	}
	return decode_ace(xdr, NULL, res->server->nfs_client);
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}