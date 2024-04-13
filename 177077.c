static int gfs2_rgrp_bh_get(struct gfs2_rgrpd *rgd)
{
	struct gfs2_sbd *sdp = rgd->rd_sbd;
	struct gfs2_glock *gl = rgd->rd_gl;
	unsigned int length = rgd->rd_length;
	struct gfs2_bitmap *bi;
	unsigned int x, y;
	int error;

	if (rgd->rd_bits[0].bi_bh != NULL)
		return 0;

	for (x = 0; x < length; x++) {
		bi = rgd->rd_bits + x;
		error = gfs2_meta_read(gl, rgd->rd_addr + x, 0, 0, &bi->bi_bh);
		if (error)
			goto fail;
	}

	for (y = length; y--;) {
		bi = rgd->rd_bits + y;
		error = gfs2_meta_wait(sdp, bi->bi_bh);
		if (error)
			goto fail;
		if (gfs2_metatype_check(sdp, bi->bi_bh, y ? GFS2_METATYPE_RB :
					      GFS2_METATYPE_RG)) {
			error = -EIO;
			goto fail;
		}
	}

	if (!(rgd->rd_flags & GFS2_RDF_UPTODATE)) {
		for (x = 0; x < length; x++)
			clear_bit(GBF_FULL, &rgd->rd_bits[x].bi_flags);
		gfs2_rgrp_in(rgd, (rgd->rd_bits[0].bi_bh)->b_data);
		rgd->rd_flags |= (GFS2_RDF_UPTODATE | GFS2_RDF_CHECK);
		rgd->rd_free_clone = rgd->rd_free;
		/* max out the rgrp allocation failure point */
		rgd->rd_extfail_pt = rgd->rd_free;
	}
	if (cpu_to_be32(GFS2_MAGIC) != rgd->rd_rgl->rl_magic) {
		rgd->rd_rgl->rl_unlinked = cpu_to_be32(count_unlinked(rgd));
		gfs2_rgrp_ondisk2lvb(rgd->rd_rgl,
				     rgd->rd_bits[0].bi_bh->b_data);
	}
	else if (sdp->sd_args.ar_rgrplvb) {
		if (!gfs2_rgrp_lvb_valid(rgd)){
			gfs2_consist_rgrpd(rgd);
			error = -EIO;
			goto fail;
		}
		if (rgd->rd_rgl->rl_unlinked == 0)
			rgd->rd_flags &= ~GFS2_RDF_CHECK;
	}
	return 0;

fail:
	while (x--) {
		bi = rgd->rd_bits + x;
		brelse(bi->bi_bh);
		bi->bi_bh = NULL;
		gfs2_assert_warn(sdp, !bi->bi_clone);
	}

	return error;
}