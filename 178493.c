static int ldb_comparison_Integer(struct ldb_context *ldb, void *mem_ctx,
				  const struct ldb_val *v1, const struct ldb_val *v2)
{
	int64_t i1=0, i2=0;
	val_to_int64(v1, &i1);
	val_to_int64(v2, &i2);
	if (i1 == i2) return 0;
	return i1 > i2? 1 : -1;
}