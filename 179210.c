prune_bounding_set(void)
{
	int i, rc = 0;
	static int bounding_set_cleared;

	if (bounding_set_cleared)
		return 0;

	for (i = 0; i <= CAP_LAST_CAP && rc == 0; ++i)
		rc = prctl(PR_CAPBSET_DROP, i);

	if (rc != 0) {
		fprintf(stderr, "Unable to clear capability bounding set: %d\n", rc);
		return EX_SYSERR;
	}

	++bounding_set_cleared;
	return 0;
}