static int decode_chan_attrs(struct xdr_stream *xdr,
			     struct nfs4_channel_attrs *attrs)
{
	__be32 *p;
	u32 nr_attrs, val;

	p = xdr_inline_decode(xdr, 28);
	if (unlikely(!p))
		goto out_overflow;
	val = be32_to_cpup(p++);	/* headerpadsz */
	if (val)
		return -EINVAL;		/* no support for header padding yet */
	attrs->max_rqst_sz = be32_to_cpup(p++);
	attrs->max_resp_sz = be32_to_cpup(p++);
	attrs->max_resp_sz_cached = be32_to_cpup(p++);
	attrs->max_ops = be32_to_cpup(p++);
	attrs->max_reqs = be32_to_cpup(p++);
	nr_attrs = be32_to_cpup(p);
	if (unlikely(nr_attrs > 1)) {
		printk(KERN_WARNING "NFS: %s: Invalid rdma channel attrs "
			"count %u\n", __func__, nr_attrs);
		return -EINVAL;
	}
	if (nr_attrs == 1) {
		p = xdr_inline_decode(xdr, 4); /* skip rdma_attrs */
		if (unlikely(!p))
			goto out_overflow;
	}
	return 0;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}