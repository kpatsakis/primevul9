int ldb_sqlite3_init(const char *version)
{
	LDB_MODULE_CHECK_VERSION(version);
	return ldb_register_backend("sqlite3", lsqlite3_connect, false);
}