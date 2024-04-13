static long long lsqlite3_get_eid_ndn(sqlite3 *sqlite, void *mem_ctx, const char *norm_dn)
{
	char *errmsg;
	char *query;
	long long eid = -1;
	long long ret;

	/* get object eid */
	query = lsqlite3_tprintf(mem_ctx, "SELECT eid "
					  "FROM ldb_entry "
					  "WHERE norm_dn = '%q';", norm_dn);
	if (query == NULL) return -1;

	ret = sqlite3_exec(sqlite, query, lsqlite3_eid_callback, &eid, &errmsg);
	if (ret != SQLITE_OK) {
		if (errmsg) {
			printf("lsqlite3_get_eid: Fatal Error: %s\n", errmsg);
			free(errmsg);
		}
		return -1;
	}

	return eid;
}