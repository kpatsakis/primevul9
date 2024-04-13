int ldb_any_comparison(struct ldb_context *ldb, void *mem_ctx, 
		       ldb_attr_handler_t canonicalise_fn, 
		       const struct ldb_val *v1,
		       const struct ldb_val *v2)
{
	int ret, ret1, ret2;
	struct ldb_val v1_canon, v2_canon;
	TALLOC_CTX *tmp_ctx = talloc_new(mem_ctx);

	/* I could try and bail if tmp_ctx was NULL, but what return
	 * value would I use?
	 *
	 * It seems easier to continue on the NULL context 
	 */
	ret1 = canonicalise_fn(ldb, tmp_ctx, v1, &v1_canon);
	ret2 = canonicalise_fn(ldb, tmp_ctx, v2, &v2_canon);

	if (ret1 == LDB_SUCCESS && ret2 == LDB_SUCCESS) {
		ret = ldb_comparison_binary(ldb, mem_ctx, &v1_canon, &v2_canon);
	} else {
		ret = ldb_comparison_binary(ldb, mem_ctx, v1, v2);
	}
	talloc_free(tmp_ctx);
	return ret;
}