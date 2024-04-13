char *ldb_binary_encode(TALLOC_CTX *mem_ctx, struct ldb_val val)
{
	size_t i;
	char *ret;
	size_t len = val.length;
	unsigned char *buf = val.data;

	for (i=0;i<val.length;i++) {
		if (need_encode(buf[i])) {
			len += 2;
		}
	}
	ret = talloc_array(mem_ctx, char, len+1);
	if (ret == NULL) return NULL;

	len = 0;
	for (i=0;i<val.length;i++) {
		if (need_encode(buf[i])) {
			snprintf(ret+len, 4, "\\%02X", buf[i]);
			len += 3;
		} else {
			ret[len++] = buf[i];
		}
	}

	ret[len] = 0;

	return ret;	
}