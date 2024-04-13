static u32 count_unlinked(struct gfs2_rgrpd *rgd)
{
	struct gfs2_bitmap *bi;
	const u32 length = rgd->rd_length;
	const u8 *buffer = NULL;
	u32 i, goal, count = 0;

	for (i = 0, bi = rgd->rd_bits; i < length; i++, bi++) {
		goal = 0;
		buffer = bi->bi_bh->b_data + bi->bi_offset;
		WARN_ON(!buffer_uptodate(bi->bi_bh));
		while (goal < bi->bi_len * GFS2_NBBY) {
			goal = gfs2_bitfit(buffer, bi->bi_len, goal,
					   GFS2_BLKST_UNLINKED);
			if (goal == BFITNOENT)
				break;
			count++;
			goal++;
		}
	}

	return count;
}