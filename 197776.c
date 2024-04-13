int blkid_probe_is_covered_by_pt(blkid_probe pr,
				 uint64_t offset, uint64_t size)
{
	blkid_probe prc = NULL;
	blkid_partlist ls = NULL;
	uint64_t start, end;
	int nparts, i, rc = 0;

	DBG(LOWPROBE, ul_debug(
		"=> checking if off=%"PRIu64" size=%"PRIu64" covered by PT",
		offset, size));

	if (pr->flags & BLKID_FL_NOSCAN_DEV)
		goto done;

	prc = blkid_clone_probe(pr);
	if (!prc)
		goto done;

	ls = blkid_probe_get_partitions(prc);
	if (!ls)
		goto done;

	nparts = blkid_partlist_numof_partitions(ls);
	if (!nparts)
		goto done;

	end = (offset + size) >> 9;
	start = offset >> 9;

	/* check if the partition table fits into the device */
	for (i = 0; i < nparts; i++) {
		blkid_partition par = &ls->parts[i];

		if (par->start + par->size > (pr->size >> 9)) {
			DBG(LOWPROBE, ul_debug("partition #%d overflows "
				"device (off=%" PRId64 " size=%" PRId64 ")",
				par->partno, par->start, par->size));
			goto done;
		}
	}

	/* check if the requested area is covered by PT */
	for (i = 0; i < nparts; i++) {
		blkid_partition par = &ls->parts[i];

		if (start >= par->start && end <= par->start + par->size) {
			rc = 1;
			break;
		}
	}
done:
	blkid_free_probe(prc);

	DBG(LOWPROBE, ul_debug("<= %s covered by PT", rc ? "IS" : "NOT"));
	return rc;
}