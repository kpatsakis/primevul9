static void try_rgrp_unlink(struct gfs2_rgrpd *rgd, u64 *last_unlinked, u64 skip)
{
	u64 block;
	struct gfs2_sbd *sdp = rgd->rd_sbd;
	struct gfs2_glock *gl;
	struct gfs2_inode *ip;
	int error;
	int found = 0;
	struct gfs2_rbm rbm = { .rgd = rgd, .bii = 0, .offset = 0 };

	while (1) {
		down_write(&sdp->sd_log_flush_lock);
		error = gfs2_rbm_find(&rbm, GFS2_BLKST_UNLINKED, NULL, NULL,
				      true);
		up_write(&sdp->sd_log_flush_lock);
		if (error == -ENOSPC)
			break;
		if (WARN_ON_ONCE(error))
			break;

		block = gfs2_rbm_to_block(&rbm);
		if (gfs2_rbm_from_block(&rbm, block + 1))
			break;
		if (*last_unlinked != NO_BLOCK && block <= *last_unlinked)
			continue;
		if (block == skip)
			continue;
		*last_unlinked = block;

		error = gfs2_glock_get(sdp, block, &gfs2_iopen_glops, CREATE, &gl);
		if (error)
			continue;

		/* If the inode is already in cache, we can ignore it here
		 * because the existing inode disposal code will deal with
		 * it when all refs have gone away. Accessing gl_object like
		 * this is not safe in general. Here it is ok because we do
		 * not dereference the pointer, and we only need an approx
		 * answer to whether it is NULL or not.
		 */
		ip = gl->gl_object;

		if (ip || queue_work(gfs2_delete_workqueue, &gl->gl_delete) == 0)
			gfs2_glock_put(gl);
		else
			found++;

		/* Limit reclaim to sensible number of tasks */
		if (found > NR_CPUS)
			return;
	}

	rgd->rd_flags &= ~GFS2_RDF_CHECK;
	return;
}