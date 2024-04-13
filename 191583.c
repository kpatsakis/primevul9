struct ldb_dn *ldb_get_schema_basedn(struct ldb_context *ldb)
{
	void *opaque = ldb_get_opaque(ldb, "schemaNamingContext");
	return talloc_get_type(opaque, struct ldb_dn);
}