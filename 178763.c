xfs_bmap_split_indlen(
	xfs_filblks_t			ores,		/* original res. */
	xfs_filblks_t			*indlen1,	/* ext1 worst indlen */
	xfs_filblks_t			*indlen2,	/* ext2 worst indlen */
	xfs_filblks_t			avail)		/* stealable blocks */
{
	xfs_filblks_t			len1 = *indlen1;
	xfs_filblks_t			len2 = *indlen2;
	xfs_filblks_t			nres = len1 + len2; /* new total res. */
	xfs_filblks_t			stolen = 0;
	xfs_filblks_t			resfactor;

	/*
	 * Steal as many blocks as we can to try and satisfy the worst case
	 * indlen for both new extents.
	 */
	if (ores < nres && avail)
		stolen = XFS_FILBLKS_MIN(nres - ores, avail);
	ores += stolen;

	 /* nothing else to do if we've satisfied the new reservation */
	if (ores >= nres)
		return stolen;

	/*
	 * We can't meet the total required reservation for the two extents.
	 * Calculate the percent of the overall shortage between both extents
	 * and apply this percentage to each of the requested indlen values.
	 * This distributes the shortage fairly and reduces the chances that one
	 * of the two extents is left with nothing when extents are repeatedly
	 * split.
	 */
	resfactor = (ores * 100);
	do_div(resfactor, nres);
	len1 *= resfactor;
	do_div(len1, 100);
	len2 *= resfactor;
	do_div(len2, 100);
	ASSERT(len1 + len2 <= ores);
	ASSERT(len1 < *indlen1 && len2 < *indlen2);

	/*
	 * Hand out the remainder to each extent. If one of the two reservations
	 * is zero, we want to make sure that one gets a block first. The loop
	 * below starts with len1, so hand len2 a block right off the bat if it
	 * is zero.
	 */
	ores -= (len1 + len2);
	ASSERT((*indlen1 - len1) + (*indlen2 - len2) >= ores);
	if (ores && !len2 && *indlen2) {
		len2++;
		ores--;
	}
	while (ores) {
		if (len1 < *indlen1) {
			len1++;
			ores--;
		}
		if (!ores)
			break;
		if (len2 < *indlen2) {
			len2++;
			ores--;
		}
	}

	*indlen1 = len1;
	*indlen2 = len2;

	return stolen;
}