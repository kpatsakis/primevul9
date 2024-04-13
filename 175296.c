static bool __splice_segment(struct page *page, unsigned int poff,
			     unsigned int plen, unsigned int *off,
			     unsigned int *len,
			     struct splice_pipe_desc *spd, bool linear,
			     struct sock *sk,
			     struct pipe_inode_info *pipe)
{
	if (!*len)
		return true;

	/* skip this segment if already processed */
	if (*off >= plen) {
		*off -= plen;
		return false;
	}

	/* ignore any bits we already processed */
	poff += *off;
	plen -= *off;
	*off = 0;

	do {
		unsigned int flen = min(*len, plen);

		if (spd_fill_page(spd, pipe, page, &flen, poff,
				  linear, sk))
			return true;
		poff += flen;
		plen -= flen;
		*len -= flen;
	} while (*len && plen);

	return false;
}