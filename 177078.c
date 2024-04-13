static int gfs2_reservation_check_and_update(struct gfs2_rbm *rbm,
					     const struct gfs2_inode *ip,
					     u32 minext,
					     struct gfs2_extent *maxext)
{
	u64 block = gfs2_rbm_to_block(rbm);
	u32 extlen = 1;
	u64 nblock;
	int ret;

	/*
	 * If we have a minimum extent length, then skip over any extent
	 * which is less than the min extent length in size.
	 */
	if (minext) {
		extlen = gfs2_free_extlen(rbm, minext);
		if (extlen <= maxext->len)
			goto fail;
	}

	/*
	 * Check the extent which has been found against the reservations
	 * and skip if parts of it are already reserved
	 */
	nblock = gfs2_next_unreserved_block(rbm->rgd, block, extlen, ip);
	if (nblock == block) {
		if (!minext || extlen >= minext)
			return 0;

		if (extlen > maxext->len) {
			maxext->len = extlen;
			maxext->rbm = *rbm;
		}
fail:
		nblock = block + extlen;
	}
	ret = gfs2_rbm_from_block(rbm, nblock);
	if (ret < 0)
		return ret;
	return 1;
}