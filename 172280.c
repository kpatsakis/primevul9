static int destructor(struct lsqlite3_private *lsqlite3)
{
	if (lsqlite3->sqlite) {
		sqlite3_close(lsqlite3->sqlite);
	}
	return 0;
}