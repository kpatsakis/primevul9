int commonio_setname (struct commonio_db *db, const char *name)
{
	snprintf (db->filename, sizeof (db->filename), "%s", name);
	return 1;
}