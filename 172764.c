static int mpage_map_and_submit_buffers(struct mpage_da_data *mpd)
{
	struct pagevec pvec;
	int nr_pages, i;
	struct inode *inode = mpd->inode;
	int bpp_bits = PAGE_SHIFT - inode->i_blkbits;
	pgoff_t start, end;
	ext4_lblk_t lblk;
	ext4_fsblk_t pblock;
	int err;
	bool map_bh = false;

	start = mpd->map.m_lblk >> bpp_bits;
	end = (mpd->map.m_lblk + mpd->map.m_len - 1) >> bpp_bits;
	lblk = start << bpp_bits;
	pblock = mpd->map.m_pblk;

	pagevec_init(&pvec);
	while (start <= end) {
		nr_pages = pagevec_lookup_range(&pvec, inode->i_mapping,
						&start, end);
		if (nr_pages == 0)
			break;
		for (i = 0; i < nr_pages; i++) {
			struct page *page = pvec.pages[i];

			err = mpage_process_page(mpd, page, &lblk, &pblock,
						 &map_bh);
			/*
			 * If map_bh is true, means page may require further bh
			 * mapping, or maybe the page was submitted for IO.
			 * So we return to call further extent mapping.
			 */
			if (err < 0 || map_bh == true)
				goto out;
			/* Page fully mapped - let IO run! */
			err = mpage_submit_page(mpd, page);
			if (err < 0)
				goto out;
		}
		pagevec_release(&pvec);
	}
	/* Extent fully mapped and matches with page boundary. We are done. */
	mpd->map.m_len = 0;
	mpd->map.m_flags = 0;
	return 0;
out:
	pagevec_release(&pvec);
	return err;
}