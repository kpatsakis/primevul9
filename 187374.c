static int decode_op_hdr(struct xdr_stream *xdr, enum nfs_opnum4 expected)
{
	__be32 *p;
	uint32_t opnum;
	int32_t nfserr;

	p = xdr_inline_decode(xdr, 8);
	if (unlikely(!p))
		goto out_overflow;
	opnum = be32_to_cpup(p++);
	if (opnum != expected) {
		dprintk("nfs: Server returned operation"
			" %d but we issued a request for %d\n",
				opnum, expected);
		return -EIO;
	}
	nfserr = be32_to_cpup(p);
	if (nfserr != NFS_OK)
		return nfs4_stat_to_errno(nfserr);
	return 0;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}