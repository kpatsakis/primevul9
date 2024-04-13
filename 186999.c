static int decode_read(struct xdr_stream *xdr, struct rpc_rqst *req, struct nfs_readres *res)
{
	__be32 *p;
	uint32_t count, eof, recvd;
	int status;

	status = decode_op_hdr(xdr, OP_READ);
	if (status)
		return status;
	p = xdr_inline_decode(xdr, 8);
	if (unlikely(!p))
		goto out_overflow;
	eof = be32_to_cpup(p++);
	count = be32_to_cpup(p);
	recvd = xdr_read_pages(xdr, count);
	if (count > recvd) {
		dprintk("NFS: server cheating in read reply: "
				"count %u > recvd %u\n", count, recvd);
		count = recvd;
		eof = 0;
	}
	res->eof = eof;
	res->count = count;
	return 0;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}