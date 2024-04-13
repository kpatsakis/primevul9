static /*@dependent@*/ /*@null@*/struct commonio_entry *find_entry_by_name (
	struct commonio_db *db,
	const char *name)
{
	return next_entry_by_name (db, db->head, name);
}