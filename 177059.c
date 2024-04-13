static int gfs2_rgrp_lvb_valid(struct gfs2_rgrpd *rgd)
{
	struct gfs2_rgrp_lvb *rgl = rgd->rd_rgl;
	struct gfs2_rgrp *str = (struct gfs2_rgrp *)rgd->rd_bits[0].bi_bh->b_data;

	if (rgl->rl_flags != str->rg_flags || rgl->rl_free != str->rg_free ||
	    rgl->rl_dinodes != str->rg_dinodes ||
	    rgl->rl_igeneration != str->rg_igeneration)
		return 0;
	return 1;
}