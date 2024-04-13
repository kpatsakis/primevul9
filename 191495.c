int commonio_rewind (struct commonio_db *db)
{
	if (!db->isopen) {
		errno = EINVAL;
		return 0;
	}
	db->cursor = NULL;
	return 1;
}