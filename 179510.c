eui64_to_str(wmem_allocator_t *scope, const guint64 ad) {
	gchar *buf, *tmp;
	guint8 *p_eui64;

	p_eui64=(guint8 *)wmem_alloc(NULL, 8);
	buf=(gchar *)wmem_alloc(scope, EUI64_STR_LEN);

	/* Copy and convert the address to network byte order. */
	*(guint64 *)(void *)(p_eui64) = pntoh64(&(ad));

	tmp = bytes_to_hexstr_punct(buf, p_eui64, 8, ':');
	*tmp = '\0'; /* NULL terminate */
	wmem_free(NULL, p_eui64);
	return buf;
}