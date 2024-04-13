static void gfs2_rgrp_out(struct gfs2_rgrpd *rgd, void *buf)
{
	struct gfs2_rgrp *str = buf;

	str->rg_flags = cpu_to_be32(rgd->rd_flags & ~GFS2_RDF_MASK);
	str->rg_free = cpu_to_be32(rgd->rd_free);
	str->rg_dinodes = cpu_to_be32(rgd->rd_dinodes);
	str->__pad = cpu_to_be32(0);
	str->rg_igeneration = cpu_to_be64(rgd->rd_igeneration);
	memset(&str->rg_reserved, 0, sizeof(str->rg_reserved));
}