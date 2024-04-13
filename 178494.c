static int ldb_comparison_dn(struct ldb_context *ldb, void *mem_ctx,
			     const struct ldb_val *v1, const struct ldb_val *v2)
{
	struct ldb_dn *dn1 = NULL, *dn2 = NULL;
	int ret;

	dn1 = ldb_dn_from_ldb_val(mem_ctx, ldb, v1);
	if ( ! ldb_dn_validate(dn1)) return -1;

	dn2 = ldb_dn_from_ldb_val(mem_ctx, ldb, v2);
	if ( ! ldb_dn_validate(dn2)) {
		talloc_free(dn1);
		return -1;
	} 

	ret = ldb_dn_compare(dn1, dn2);

	talloc_free(dn1);
	talloc_free(dn2);
	return ret;
}