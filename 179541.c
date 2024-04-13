bytes_to_str(wmem_allocator_t *scope, const guint8 *bd, int bd_len)
{
	gchar *cur;
	gchar *cur_ptr;
	int truncated = 0;

	if (!bd)
		REPORT_DISSECTOR_BUG("Null pointer passed to bytes_to_str()");

	cur=(gchar *)wmem_alloc(scope, MAX_BYTE_STR_LEN+3+1);
	if (bd_len <= 0) { cur[0] = '\0'; return cur; }

	if (bd_len > MAX_BYTE_STR_LEN/2) {	/* bd_len > 24 */
		truncated = 1;
		bd_len = MAX_BYTE_STR_LEN/2;
	}

	cur_ptr = bytes_to_hexstr(cur, bd, bd_len);	/* max MAX_BYTE_STR_LEN bytes */

	if (truncated)
		cur_ptr = g_stpcpy(cur_ptr, UTF8_HORIZONTAL_ELLIPSIS);	/* 3 bytes */

	*cur_ptr = '\0';				/* 1 byte */
	return cur;
}