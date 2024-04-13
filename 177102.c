static void gfs2_adjust_reservation(struct gfs2_inode *ip,
				    const struct gfs2_rbm *rbm, unsigned len)
{
	struct gfs2_blkreserv *rs = &ip->i_res;
	struct gfs2_rgrpd *rgd = rbm->rgd;
	unsigned rlen;
	u64 block;
	int ret;

	spin_lock(&rgd->rd_rsspin);
	if (gfs2_rs_active(rs)) {
		if (gfs2_rbm_eq(&rs->rs_rbm, rbm)) {
			block = gfs2_rbm_to_block(rbm);
			ret = gfs2_rbm_from_block(&rs->rs_rbm, block + len);
			rlen = min(rs->rs_free, len);
			rs->rs_free -= rlen;
			rgd->rd_reserved -= rlen;
			trace_gfs2_rs(rs, TRACE_RS_CLAIM);
			if (rs->rs_free && !ret)
				goto out;
			/* We used up our block reservation, so we should
			   reserve more blocks next time. */
			atomic_add(RGRP_RSRV_ADDBLKS, &rs->rs_sizehint);
		}
		__rs_deltree(rs);
	}
out:
	spin_unlock(&rgd->rd_rsspin);
}