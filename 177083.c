static void gfs2_rgrp_ondisk2lvb(struct gfs2_rgrp_lvb *rgl, const void *buf)
{
	const struct gfs2_rgrp *str = buf;

	rgl->rl_magic = cpu_to_be32(GFS2_MAGIC);
	rgl->rl_flags = str->rg_flags;
	rgl->rl_free = str->rg_free;
	rgl->rl_dinodes = str->rg_dinodes;
	rgl->rl_igeneration = str->rg_igeneration;
	rgl->__pad = 0UL;
}