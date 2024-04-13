static int decode_first_threshold_item4(struct xdr_stream *xdr,
					struct nfs4_threshold *res)
{
	__be32 *p;
	unsigned int savep;
	uint32_t bitmap[3] = {0,}, attrlen;
	int status;

	/* layout type */
	p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p)) {
		print_overflow_msg(__func__, xdr);
		return -EIO;
	}
	res->l_type = be32_to_cpup(p);

	/* thi_hintset bitmap */
	status = decode_attr_bitmap(xdr, bitmap);
	if (status < 0)
		goto xdr_error;

	/* thi_hintlist length */
	status = decode_attr_length(xdr, &attrlen, &savep);
	if (status < 0)
		goto xdr_error;
	/* thi_hintlist */
	status = decode_threshold_hint(xdr, bitmap, &res->rd_sz, THRESHOLD_RD);
	if (status < 0)
		goto xdr_error;
	status = decode_threshold_hint(xdr, bitmap, &res->wr_sz, THRESHOLD_WR);
	if (status < 0)
		goto xdr_error;
	status = decode_threshold_hint(xdr, bitmap, &res->rd_io_sz,
				       THRESHOLD_RD_IO);
	if (status < 0)
		goto xdr_error;
	status = decode_threshold_hint(xdr, bitmap, &res->wr_io_sz,
				       THRESHOLD_WR_IO);
	if (status < 0)
		goto xdr_error;

	status = verify_attr_len(xdr, savep, attrlen);
	res->bm = bitmap[0];

	dprintk("%s bm=0x%x rd_sz=%llu wr_sz=%llu rd_io=%llu wr_io=%llu\n",
		 __func__, res->bm, res->rd_sz, res->wr_sz, res->rd_io_sz,
		res->wr_io_sz);
xdr_error:
	dprintk("%s ret=%d!\n", __func__, status);
	return status;
}