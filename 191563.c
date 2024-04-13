struct ldb_dn *ldb_get_default_basedn(struct ldb_context *ldb)
{
	void *opaque = ldb_get_opaque(ldb, "defaultNamingContext");
	return talloc_get_type(opaque, struct ldb_dn);
}