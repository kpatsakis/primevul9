static int decode_layoutget(struct xdr_stream *xdr, struct rpc_rqst *req,
			    struct nfs4_layoutget_res *res)
{
	__be32 *p;
	int status;
	u32 layout_count;
	u32 recvd;

	status = decode_op_hdr(xdr, OP_LAYOUTGET);
	if (status)
		return status;
	p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p))
		goto out_overflow;
	res->return_on_close = be32_to_cpup(p);
	decode_stateid(xdr, &res->stateid);
	p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p))
		goto out_overflow;
	layout_count = be32_to_cpup(p);
	if (!layout_count) {
		dprintk("%s: server responded with empty layout array\n",
			__func__);
		return -EINVAL;
	}

	p = xdr_inline_decode(xdr, 28);
	if (unlikely(!p))
		goto out_overflow;
	p = xdr_decode_hyper(p, &res->range.offset);
	p = xdr_decode_hyper(p, &res->range.length);
	res->range.iomode = be32_to_cpup(p++);
	res->type = be32_to_cpup(p++);
	res->layoutp->len = be32_to_cpup(p);

	dprintk("%s roff:%lu rlen:%lu riomode:%d, lo_type:0x%x, lo.len:%d\n",
		__func__,
		(unsigned long)res->range.offset,
		(unsigned long)res->range.length,
		res->range.iomode,
		res->type,
		res->layoutp->len);

	recvd = xdr_read_pages(xdr, res->layoutp->len);
	if (res->layoutp->len > recvd) {
		dprintk("NFS: server cheating in layoutget reply: "
				"layout len %u > recvd %u\n",
				res->layoutp->len, recvd);
		return -EINVAL;
	}

	if (layout_count > 1) {
		/* We only handle a length one array at the moment.  Any
		 * further entries are just ignored.  Note that this means
		 * the client may see a response that is less than the
		 * minimum it requested.
		 */
		dprintk("%s: server responded with %d layouts, dropping tail\n",
			__func__, layout_count);
	}

	return 0;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}