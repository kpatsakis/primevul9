int ldb_comparison_fold(struct ldb_context *ldb, void *mem_ctx,
			       const struct ldb_val *v1, const struct ldb_val *v2)
{
	const char *s1=(const char *)v1->data, *s2=(const char *)v2->data;
	size_t n1 = v1->length, n2 = v2->length;
	char *b1, *b2;
	const char *u1, *u2;
	int ret;
	while (n1 && *s1 == ' ') { s1++; n1--; };
	while (n2 && *s2 == ' ') { s2++; n2--; };

	while (n1 && n2 && *s1 && *s2) {
		/* the first 127 (0x7F) chars are ascii and utf8 guarantes they
		 * never appear in multibyte sequences */
		if (((unsigned char)s1[0]) & 0x80) goto utf8str;
		if (((unsigned char)s2[0]) & 0x80) goto utf8str;
		if (toupper((unsigned char)*s1) != toupper((unsigned char)*s2))
			break;
		if (*s1 == ' ') {
			while (n1 && s1[0] == s1[1]) { s1++; n1--; }
			while (n2 && s2[0] == s2[1]) { s2++; n2--; }
		}
		s1++; s2++;
		n1--; n2--;
	}

	/* check for trailing spaces only if the other pointers has
	 * reached the end of the strings otherwise we can
	 * mistakenly match.  ex. "domain users" <->
	 * "domainUpdates"
	 */
	if (n1 && *s1 == ' ' && (!n2 || !*s2)) {
		while (n1 && *s1 == ' ') { s1++; n1--; }		
	}
	if (n2 && *s2 == ' ' && (!n1 || !*s1)) {
		while (n2 && *s2 == ' ') { s2++; n2--; }		
	}
	if (n1 == 0 && n2 != 0) {
		return -(int)toupper(*s2);
	}
	if (n2 == 0 && n1 != 0) {
		return (int)toupper(*s1);
	}
	if (n1 == 0 && n2 == 0) {
		return 0;
	}
	return (int)toupper(*s1) - (int)toupper(*s2);

utf8str:
	/* no need to recheck from the start, just from the first utf8 char found */
	b1 = ldb_casefold(ldb, mem_ctx, s1, n1);
	b2 = ldb_casefold(ldb, mem_ctx, s2, n2);

	if (!b1 || !b2) {
		/* One of the strings was not UTF8, so we have no
		 * options but to do a binary compare */
		talloc_free(b1);
		talloc_free(b2);
		ret = memcmp(s1, s2, MIN(n1, n2));
		if (ret == 0) {
			if (n1 == n2) return 0;
			if (n1 > n2) {
				return (int)toupper(s1[n2]);
			} else {
				return -(int)toupper(s2[n1]);
			}
		}
		return ret;
	}

	u1 = b1;
	u2 = b2;

	while (*u1 & *u2) {
		if (*u1 != *u2)
			break;
		if (*u1 == ' ') {
			while (u1[0] == u1[1]) u1++;
			while (u2[0] == u2[1]) u2++;
		}
		u1++; u2++;
	}
	if (! (*u1 && *u2)) {
		while (*u1 == ' ') u1++;
		while (*u2 == ' ') u2++;
	}
	ret = (int)(*u1 - *u2);

	talloc_free(b1);
	talloc_free(b2);
	
	return ret;
}