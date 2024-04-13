static void __rs_deltree(struct gfs2_blkreserv *rs)
{
	struct gfs2_rgrpd *rgd;

	if (!gfs2_rs_active(rs))
		return;

	rgd = rs->rs_rbm.rgd;
	trace_gfs2_rs(rs, TRACE_RS_TREEDEL);
	rb_erase(&rs->rs_node, &rgd->rd_rstree);
	RB_CLEAR_NODE(&rs->rs_node);

	if (rs->rs_free) {
		struct gfs2_bitmap *bi = rbm_bi(&rs->rs_rbm);

		/* return reserved blocks to the rgrp */
		BUG_ON(rs->rs_rbm.rgd->rd_reserved < rs->rs_free);
		rs->rs_rbm.rgd->rd_reserved -= rs->rs_free;
		/* The rgrp extent failure point is likely not to increase;
		   it will only do so if the freed blocks are somehow
		   contiguous with a span of free blocks that follows. Still,
		   it will force the number to be recalculated later. */
		rgd->rd_extfail_pt += rs->rs_free;
		rs->rs_free = 0;
		clear_bit(GBF_FULL, &bi->bi_flags);
	}
}