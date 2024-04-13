static int decode_space_limit(struct xdr_stream *xdr, u64 *maxsize)
{
	__be32 *p;
	uint32_t limit_type, nblocks, blocksize;

	p = xdr_inline_decode(xdr, 12);
	if (unlikely(!p))
		goto out_overflow;
	limit_type = be32_to_cpup(p++);
	switch (limit_type) {
	case 1:
		xdr_decode_hyper(p, maxsize);
		break;
	case 2:
		nblocks = be32_to_cpup(p++);
		blocksize = be32_to_cpup(p);
		*maxsize = (uint64_t)nblocks * (uint64_t)blocksize;
	}
	return 0;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}