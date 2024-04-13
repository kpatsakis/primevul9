int nfs4_decode_dirent(struct xdr_stream *xdr, struct nfs_entry *entry,
		       int plus)
{
	unsigned int savep;
	uint32_t bitmap[3] = {0};
	uint32_t len;
	__be32 *p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p))
		goto out_overflow;
	if (*p == xdr_zero) {
		p = xdr_inline_decode(xdr, 4);
		if (unlikely(!p))
			goto out_overflow;
		if (*p == xdr_zero)
			return -EAGAIN;
		entry->eof = 1;
		return -EBADCOOKIE;
	}

	p = xdr_inline_decode(xdr, 12);
	if (unlikely(!p))
		goto out_overflow;
	entry->prev_cookie = entry->cookie;
	p = xdr_decode_hyper(p, &entry->cookie);
	entry->len = be32_to_cpup(p);

	p = xdr_inline_decode(xdr, entry->len);
	if (unlikely(!p))
		goto out_overflow;
	entry->name = (const char *) p;

	/*
	 * In case the server doesn't return an inode number,
	 * we fake one here.  (We don't use inode number 0,
	 * since glibc seems to choke on it...)
	 */
	entry->ino = 1;
	entry->fattr->valid = 0;

	if (decode_attr_bitmap(xdr, bitmap) < 0)
		goto out_overflow;

	if (decode_attr_length(xdr, &len, &savep) < 0)
		goto out_overflow;

	if (decode_getfattr_attrs(xdr, bitmap, entry->fattr, entry->fh,
				  NULL, entry->server) < 0)
		goto out_overflow;
	if (entry->fattr->valid & NFS_ATTR_FATTR_MOUNTED_ON_FILEID)
		entry->ino = entry->fattr->mounted_on_fileid;
	else if (entry->fattr->valid & NFS_ATTR_FATTR_FILEID)
		entry->ino = entry->fattr->fileid;

	entry->d_type = DT_UNKNOWN;
	if (entry->fattr->valid & NFS_ATTR_FATTR_TYPE)
		entry->d_type = nfs_umode_to_dtype(entry->fattr->mode);

	return 0;

out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EAGAIN;
}