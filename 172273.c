static int lsqlite3_eid_callback(void *result, int col_num, char **cols, char **names)
{
	long long *eid = (long long *)result;

	if (col_num != 1) return SQLITE_ABORT;
	if (strcasecmp(names[0], "eid") != 0) return SQLITE_ABORT;

	*eid = atoll(cols[0]);
	return SQLITE_OK;
}