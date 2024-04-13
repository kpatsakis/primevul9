static int ldb_comparison_utctime(struct ldb_context *ldb, void *mem_ctx,
				  const struct ldb_val *v1, const struct ldb_val *v2)
{
	time_t t1=0, t2=0;
	ldb_val_to_time(v1, &t1);
	ldb_val_to_time(v2, &t2);
	if (t1 == t2) return 0;
	return t1 > t2? 1 : -1;
}