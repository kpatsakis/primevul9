static void update_rgrp_lvb_unlinked(struct gfs2_rgrpd *rgd, u32 change)
{
	struct gfs2_rgrp_lvb *rgl = rgd->rd_rgl;
	u32 unlinked = be32_to_cpu(rgl->rl_unlinked) + change;
	rgl->rl_unlinked = cpu_to_be32(unlinked);
}