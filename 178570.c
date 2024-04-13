static zend_string *unserialize_str(const unsigned char **p, size_t len, size_t maxlen)
{
	size_t i, j;
	zend_string *str = zend_string_alloc(len, 0);
	unsigned char *end = *(unsigned char **)p+maxlen;

	if (end < *p) {
		zend_string_free(str);
		return NULL;
	}

	for (i = 0; i < len; i++) {
		if (*p >= end) {
			zend_string_free(str);
			return NULL;
		}
		if (**p != '\\') {
			str->val[i] = (char)**p;
		} else {
			unsigned char ch = 0;

			for (j = 0; j < 2; j++) {
				(*p)++;
				if (**p >= '0' && **p <= '9') {
					ch = (ch << 4) + (**p -'0');
				} else if (**p >= 'a' && **p <= 'f') {
					ch = (ch << 4) + (**p -'a'+10);
				} else if (**p >= 'A' && **p <= 'F') {
					ch = (ch << 4) + (**p -'A'+10);
				} else {
					zend_string_free(str);
					return NULL;
				}
			}
			str->val[i] = (char)ch;
		}
		(*p)++;
	}
	str->val[i] = 0;
	str->len = i;
	return str;
}