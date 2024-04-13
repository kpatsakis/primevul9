static int update_rgrp_lvb(struct gfs2_rgrpd *rgd)
{
	u32 rl_flags;

	if (rgd->rd_flags & GFS2_RDF_UPTODATE)
		return 0;

	if (cpu_to_be32(GFS2_MAGIC) != rgd->rd_rgl->rl_magic)
		return gfs2_rgrp_bh_get(rgd);

	rl_flags = be32_to_cpu(rgd->rd_rgl->rl_flags);
	rl_flags &= ~GFS2_RDF_MASK;
	rgd->rd_flags &= GFS2_RDF_MASK;
	rgd->rd_flags |= (rl_flags | GFS2_RDF_UPTODATE | GFS2_RDF_CHECK);
	if (rgd->rd_rgl->rl_unlinked == 0)
		rgd->rd_flags &= ~GFS2_RDF_CHECK;
	rgd->rd_free = be32_to_cpu(rgd->rd_rgl->rl_free);
	rgd->rd_free_clone = rgd->rd_free;
	rgd->rd_dinodes = be32_to_cpu(rgd->rd_rgl->rl_dinodes);
	rgd->rd_igeneration = be64_to_cpu(rgd->rd_rgl->rl_igeneration);
	return 0;
}