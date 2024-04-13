char *ldb_binary_encode_string(TALLOC_CTX *mem_ctx, const char *string)
{
	struct ldb_val val;
	if (string == NULL) {
		return NULL;
	}
	val.data = discard_const_p(uint8_t, string);
	val.length = strlen(string);
	return ldb_binary_encode(mem_ctx, val);
}