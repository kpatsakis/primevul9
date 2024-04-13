static int decode_secinfo_gss(struct xdr_stream *xdr, struct nfs4_secinfo_flavor *flavor)
{
	__be32 *p;

	p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p))
		goto out_overflow;
	flavor->gss.sec_oid4.len = be32_to_cpup(p);
	if (flavor->gss.sec_oid4.len > GSS_OID_MAX_LEN)
		goto out_err;

	p = xdr_inline_decode(xdr, flavor->gss.sec_oid4.len);
	if (unlikely(!p))
		goto out_overflow;
	memcpy(flavor->gss.sec_oid4.data, p, flavor->gss.sec_oid4.len);

	p = xdr_inline_decode(xdr, 8);
	if (unlikely(!p))
		goto out_overflow;
	flavor->gss.qop4 = be32_to_cpup(p++);
	flavor->gss.service = be32_to_cpup(p);

	return 0;

out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
out_err:
	return -EINVAL;
}