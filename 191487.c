int commonio_lock (struct commonio_db *db)
{
#ifdef HAVE_LCKPWDF
	/*
	 * only if the system libc has a real lckpwdf() - the one from
	 * lockpw.c calls us and would cause infinite recursion!
	 */

	/*
	 * Call lckpwdf() on the first lock.
	 * If it succeeds, call *_lock() only once
	 * (no retries, it should always succeed).
	 */
	if (0 == lock_count) {
		if (lckpwdf () == -1) {
			if (geteuid () != 0) {
				(void) fprintf (stderr,
				                "%s: Permission denied.\n",
				                Prog);
			}
			return 0;	/* failure */
		}
	}

	if (commonio_lock_nowait (db, true) != 0) {
		return 1;	/* success */
	}

	ulckpwdf ();
	return 0;		/* failure */
#else				/* !HAVE_LCKPWDF */
	int i;

	/*
	 * lckpwdf() not used - do it the old way.
	 */
#ifndef LOCK_TRIES
#define LOCK_TRIES 15
#endif

#ifndef LOCK_SLEEP
#define LOCK_SLEEP 1
#endif
	for (i = 0; i < LOCK_TRIES; i++) {
		if (i > 0) {
			sleep (LOCK_SLEEP);	/* delay between retries */
		}
		if (commonio_lock_nowait (db, i==LOCK_TRIES-1) != 0) {
			return 1;	/* success */
		}
		/* no unnecessary retries on "permission denied" errors */
		if (geteuid () != 0) {
			(void) fprintf (stderr, "%s: Permission denied.\n",
			                Prog);
			return 0;
		}
	}
	return 0;		/* failure */
#endif				/* !HAVE_LCKPWDF */
}