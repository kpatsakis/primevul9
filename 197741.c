static int partitions_probe(blkid_probe pr, struct blkid_chain *chn)
{
	int rc = BLKID_PROBE_NONE;
	size_t i;

	if (!pr || chn->idx < -1)
		return -EINVAL;

	blkid_probe_chain_reset_values(pr, chn);

	if (pr->flags & BLKID_FL_NOSCAN_DEV)
		return BLKID_PROBE_NONE;

	if (chn->binary)
		partitions_init_data(chn);

	if (!pr->wipe_size && (pr->prob_flags & BLKID_PROBE_FL_IGNORE_PT))
		goto details_only;

	DBG(LOWPROBE, ul_debug("--> starting probing loop [PARTS idx=%d]",
		chn->idx));

	i = chn->idx < 0 ? 0 : chn->idx + 1U;

	for ( ; i < ARRAY_SIZE(idinfos); i++) {
		const char *name;

		chn->idx = i;

		/* apply filter */
		if (chn->fltr && blkid_bmp_get_item(chn->fltr, i))
			continue;

		/* apply checks from idinfo */
		rc = idinfo_probe(pr, idinfos[i], chn);
		if (rc < 0)
			break;
		if (rc != BLKID_PROBE_OK)
			continue;

		name = idinfos[i]->name;

		if (!chn->binary)
			/*
			 * Non-binary interface, set generic variables. Note
			 * that the another variables could be set in prober
			 * functions.
			 */
			blkid_probe_set_value(pr, "PTTYPE",
						(unsigned char *) name,
						strlen(name) + 1);

		DBG(LOWPROBE, ul_debug("<-- leaving probing loop (type=%s) [PARTS idx=%d]",
			name, chn->idx));
		rc = BLKID_PROBE_OK;
		break;
	}

	if (rc != BLKID_PROBE_OK) {
		DBG(LOWPROBE, ul_debug("<-- leaving probing loop (failed=%d) [PARTS idx=%d]",
			rc, chn->idx));
	}

details_only:
	/*
	 * Gather PART_ENTRY_* values if the current device is a partition.
	 */
	if ((rc == BLKID_PROBE_OK || rc == BLKID_PROBE_NONE) && !chn->binary &&
	    (blkid_partitions_get_flags(pr) & BLKID_PARTS_ENTRY_DETAILS)) {

		int xrc = blkid_partitions_probe_partition(pr);

		/* partition entry probing is optional, and "not-found" from
		 * this sub-probing must not to overwrite previous success. */
		if (xrc < 0)
			rc = xrc;			/* always propagate errors */
		else if (rc == BLKID_PROBE_NONE)
			rc = xrc;
	}

	DBG(LOWPROBE, ul_debug("partitions probe done [rc=%d]",	rc));
	return rc;
}