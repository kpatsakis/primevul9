int commonio_lock_nowait (struct commonio_db *db, bool log)
{
	char file[1024];
	char lock[1024];

	if (db->locked) {
		return 1;
	}

	snprintf (file, sizeof file, "%s.%lu",
	          db->filename, (unsigned long) getpid ());
	snprintf (lock, sizeof lock, "%s.lock", db->filename);
	if (do_lock_file (file, lock, log) != 0) {
		db->locked = true;
		lock_count++;
		return 1;
	}
	return 0;
}