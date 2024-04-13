static int idinfo_probe(blkid_probe pr, const struct blkid_idinfo *id,
			struct blkid_chain *chn)
{
	const struct blkid_idmag *mag = NULL;
	uint64_t off;
	int rc = BLKID_PROBE_NONE;		/* default is nothing */

	if (pr->size <= 0 || (id->minsz && (unsigned)id->minsz > pr->size))
		goto nothing;	/* the device is too small */
	if (pr->flags & BLKID_FL_NOSCAN_DEV)
		goto nothing;

	rc = blkid_probe_get_idmag(pr, id, &off, &mag);
	if (rc != BLKID_PROBE_OK)
		goto nothing;

	/* final check by probing function */
	if (id->probefunc) {
		DBG(LOWPROBE, ul_debug(
			"%s: ---> call probefunc()", id->name));
		rc = id->probefunc(pr, mag);
		if (rc < 0) {
			/* reset after error */
			reset_partlist(blkid_probe_get_partlist(pr));
			if (chn && !chn->binary)
				blkid_probe_chain_reset_values(pr, chn);
			DBG(LOWPROBE, ul_debug("%s probefunc failed, rc %d",
						  id->name, rc));
		}
		if (rc == BLKID_PROBE_OK && mag && chn && !chn->binary)
			rc = blkid_probe_set_magic(pr, off, mag->len,
					(unsigned char *) mag->magic);

		DBG(LOWPROBE, ul_debug("%s: <--- (rc = %d)", id->name, rc));
	}

	return rc;

nothing:
	return BLKID_PROBE_NONE;
}