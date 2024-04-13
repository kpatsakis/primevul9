struct ldb_dn *ldb_get_root_basedn(struct ldb_context *ldb)
{
	void *opaque = ldb_get_opaque(ldb, "rootDomainNamingContext");
	return talloc_get_type(opaque, struct ldb_dn);
}