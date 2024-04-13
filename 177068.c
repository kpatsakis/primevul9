static u32 gfs2_free_extlen(const struct gfs2_rbm *rrbm, u32 len)
{
	struct gfs2_rbm rbm = *rrbm;
	u32 n_unaligned = rbm.offset & 3;
	u32 size = len;
	u32 bytes;
	u32 chunk_size;
	u8 *ptr, *start, *end;
	u64 block;
	struct gfs2_bitmap *bi;

	if (n_unaligned &&
	    gfs2_unaligned_extlen(&rbm, 4 - n_unaligned, &len))
		goto out;

	n_unaligned = len & 3;
	/* Start is now byte aligned */
	while (len > 3) {
		bi = rbm_bi(&rbm);
		start = bi->bi_bh->b_data;
		if (bi->bi_clone)
			start = bi->bi_clone;
		end = start + bi->bi_bh->b_size;
		start += bi->bi_offset;
		BUG_ON(rbm.offset & 3);
		start += (rbm.offset / GFS2_NBBY);
		bytes = min_t(u32, len / GFS2_NBBY, (end - start));
		ptr = memchr_inv(start, 0, bytes);
		chunk_size = ((ptr == NULL) ? bytes : (ptr - start));
		chunk_size *= GFS2_NBBY;
		BUG_ON(len < chunk_size);
		len -= chunk_size;
		block = gfs2_rbm_to_block(&rbm);
		if (gfs2_rbm_from_block(&rbm, block + chunk_size)) {
			n_unaligned = 0;
			break;
		}
		if (ptr) {
			n_unaligned = 3;
			break;
		}
		n_unaligned = len & 3;
	}

	/* Deal with any bits left over at the end */
	if (n_unaligned)
		gfs2_unaligned_extlen(&rbm, n_unaligned, &len);
out:
	return size - len;
}