struct ldb_dn *ldb_get_config_basedn(struct ldb_context *ldb)
{
	void *opaque = ldb_get_opaque(ldb, "configurationNamingContext");
	return talloc_get_type(opaque, struct ldb_dn);
}