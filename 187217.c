static int decode_secinfo_common(struct xdr_stream *xdr, struct nfs4_secinfo_res *res)
{
	struct nfs4_secinfo_flavor *sec_flavor;
	int status;
	__be32 *p;
	int i, num_flavors;

	p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p))
		goto out_overflow;

	res->flavors->num_flavors = 0;
	num_flavors = be32_to_cpup(p);

	for (i = 0; i < num_flavors; i++) {
		sec_flavor = &res->flavors->flavors[i];
		if ((char *)&sec_flavor[1] - (char *)res->flavors > PAGE_SIZE)
			break;

		p = xdr_inline_decode(xdr, 4);
		if (unlikely(!p))
			goto out_overflow;
		sec_flavor->flavor = be32_to_cpup(p);

		if (sec_flavor->flavor == RPC_AUTH_GSS) {
			status = decode_secinfo_gss(xdr, sec_flavor);
			if (status)
				goto out;
		}
		res->flavors->num_flavors++;
	}

	status = 0;
out:
	return status;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}