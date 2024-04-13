static void set_rgrp_preferences(struct gfs2_sbd *sdp)
{
	struct gfs2_rgrpd *rgd, *first;
	int i;

	/* Skip an initial number of rgrps, based on this node's journal ID.
	   That should start each node out on its own set. */
	rgd = gfs2_rgrpd_get_first(sdp);
	for (i = 0; i < sdp->sd_lockstruct.ls_jid; i++)
		rgd = gfs2_rgrpd_get_next(rgd);
	first = rgd;

	do {
		rgd->rd_flags |= GFS2_RDF_PREFERRED;
		for (i = 0; i < sdp->sd_journals; i++) {
			rgd = gfs2_rgrpd_get_next(rgd);
			if (!rgd || rgd == first)
				break;
		}
	} while (rgd && rgd != first);
}