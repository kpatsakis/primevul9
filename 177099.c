void gfs2_free_clones(struct gfs2_rgrpd *rgd)
{
	int x;

	for (x = 0; x < rgd->rd_length; x++) {
		struct gfs2_bitmap *bi = rgd->rd_bits + x;
		kfree(bi->bi_clone);
		bi->bi_clone = NULL;
	}
}