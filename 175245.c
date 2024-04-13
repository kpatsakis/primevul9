static bool spd_fill_page(struct splice_pipe_desc *spd,
			  struct pipe_inode_info *pipe, struct page *page,
			  unsigned int *len, unsigned int offset,
			  bool linear,
			  struct sock *sk)
{
	if (unlikely(spd->nr_pages == MAX_SKB_FRAGS))
		return true;

	if (linear) {
		page = linear_to_page(page, len, &offset, sk);
		if (!page)
			return true;
	}
	if (spd_can_coalesce(spd, page, offset)) {
		spd->partial[spd->nr_pages - 1].len += *len;
		return false;
	}
	get_page(page);
	spd->pages[spd->nr_pages] = page;
	spd->partial[spd->nr_pages].len = *len;
	spd->partial[spd->nr_pages].offset = offset;
	spd->nr_pages++;

	return false;
}