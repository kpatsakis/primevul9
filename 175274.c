static bool spd_can_coalesce(const struct splice_pipe_desc *spd,
			     struct page *page,
			     unsigned int offset)
{
	return	spd->nr_pages &&
		spd->pages[spd->nr_pages - 1] == page &&
		(spd->partial[spd->nr_pages - 1].offset +
		 spd->partial[spd->nr_pages - 1].len == offset);
}