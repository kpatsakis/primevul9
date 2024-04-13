static int decode_lock_denied (struct xdr_stream *xdr, struct file_lock *fl)
{
	uint64_t offset, length, clientid;
	__be32 *p;
	uint32_t namelen, type;

	p = xdr_inline_decode(xdr, 32); /* read 32 bytes */
	if (unlikely(!p))
		goto out_overflow;
	p = xdr_decode_hyper(p, &offset); /* read 2 8-byte long words */
	p = xdr_decode_hyper(p, &length);
	type = be32_to_cpup(p++); /* 4 byte read */
	if (fl != NULL) { /* manipulate file lock */
		fl->fl_start = (loff_t)offset;
		fl->fl_end = fl->fl_start + (loff_t)length - 1;
		if (length == ~(uint64_t)0)
			fl->fl_end = OFFSET_MAX;
		fl->fl_type = F_WRLCK;
		if (type & 1)
			fl->fl_type = F_RDLCK;
		fl->fl_pid = 0;
	}
	p = xdr_decode_hyper(p, &clientid); /* read 8 bytes */
	namelen = be32_to_cpup(p); /* read 4 bytes */  /* have read all 32 bytes now */
	p = xdr_inline_decode(xdr, namelen); /* variable size field */
	if (likely(p))
		return -NFS4ERR_DENIED;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}