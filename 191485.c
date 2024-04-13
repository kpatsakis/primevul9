bool commonio_present (const struct commonio_db *db)
{
	return (access (db->filename, F_OK) == 0);
}