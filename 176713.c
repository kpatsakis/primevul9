struct ldb_val ldb_binary_decode(TALLOC_CTX *mem_ctx, const char *str)
{
	size_t i, j;
	struct ldb_val ret;
	size_t slen = str?strlen(str):0;

	ret.data = (uint8_t *)talloc_size(mem_ctx, slen+1);
	ret.length = 0;
	if (ret.data == NULL) return ret;

	for (i=j=0;i<slen;i++) {
		if (str[i] == '\\') {
			int c;

			c = ldb_parse_hex2char(&str[i+1]);
			if (c == -1) {
				talloc_free(ret.data);
				memset(&ret, 0, sizeof(ret));
				return ret;
			}
			((uint8_t *)ret.data)[j++] = c;
			i += 2;
		} else {
			((uint8_t *)ret.data)[j++] = str[i];
		}
	}
	ret.length = j;
	((uint8_t *)ret.data)[j] = 0;

	return ret;
}