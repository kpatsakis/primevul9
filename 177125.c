static void gfs2_rgrp_in(struct gfs2_rgrpd *rgd, const void *buf)
{
	const struct gfs2_rgrp *str = buf;
	u32 rg_flags;

	rg_flags = be32_to_cpu(str->rg_flags);
	rg_flags &= ~GFS2_RDF_MASK;
	rgd->rd_flags &= GFS2_RDF_MASK;
	rgd->rd_flags |= rg_flags;
	rgd->rd_free = be32_to_cpu(str->rg_free);
	rgd->rd_dinodes = be32_to_cpu(str->rg_dinodes);
	rgd->rd_igeneration = be64_to_cpu(str->rg_igeneration);
}