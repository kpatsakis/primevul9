static int mpage_process_page(struct mpage_da_data *mpd, struct page *page,
			      ext4_lblk_t *m_lblk, ext4_fsblk_t *m_pblk,
			      bool *map_bh)
{
	struct buffer_head *head, *bh;
	ext4_io_end_t *io_end = mpd->io_submit.io_end;
	ext4_lblk_t lblk = *m_lblk;
	ext4_fsblk_t pblock = *m_pblk;
	int err = 0;
	int blkbits = mpd->inode->i_blkbits;
	ssize_t io_end_size = 0;
	struct ext4_io_end_vec *io_end_vec = ext4_last_io_end_vec(io_end);

	bh = head = page_buffers(page);
	do {
		if (lblk < mpd->map.m_lblk)
			continue;
		if (lblk >= mpd->map.m_lblk + mpd->map.m_len) {
			/*
			 * Buffer after end of mapped extent.
			 * Find next buffer in the page to map.
			 */
			mpd->map.m_len = 0;
			mpd->map.m_flags = 0;
			io_end_vec->size += io_end_size;
			io_end_size = 0;

			err = mpage_process_page_bufs(mpd, head, bh, lblk);
			if (err > 0)
				err = 0;
			if (!err && mpd->map.m_len && mpd->map.m_lblk > lblk) {
				io_end_vec = ext4_alloc_io_end_vec(io_end);
				io_end_vec->offset = mpd->map.m_lblk << blkbits;
			}
			*map_bh = true;
			goto out;
		}
		if (buffer_delay(bh)) {
			clear_buffer_delay(bh);
			bh->b_blocknr = pblock++;
		}
		clear_buffer_unwritten(bh);
		io_end_size += (1 << blkbits);
	} while (lblk++, (bh = bh->b_this_page) != head);

	io_end_vec->size += io_end_size;
	io_end_size = 0;
	*map_bh = false;
out:
	*m_lblk = lblk;
	*m_pblk = pblock;
	return err;
}