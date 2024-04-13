void gfs2_rgrp_brelse(struct gfs2_rgrpd *rgd)
{
	int x, length = rgd->rd_length;

	for (x = 0; x < length; x++) {
		struct gfs2_bitmap *bi = rgd->rd_bits + x;
		if (bi->bi_bh) {
			brelse(bi->bi_bh);
			bi->bi_bh = NULL;
		}
	}

}