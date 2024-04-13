int commonio_unlock (struct commonio_db *db)
{
	char lock[1024];

	if (db->isopen) {
		db->readonly = true;
		if (commonio_close (db) == 0) {
			if (db->locked) {
				dec_lock_count ();
			}
			return 0;
		}
	}
	if (db->locked) {
		/*
		 * Unlock in reverse order: remove the lock file,
		 * then call ulckpwdf() (if used) on last unlock.
		 */
		db->locked = false;
		snprintf (lock, sizeof lock, "%s.lock", db->filename);
		unlink (lock);
		dec_lock_count ();
		return 1;
	}
	return 0;
}