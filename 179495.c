signed_time_secs_to_str(wmem_allocator_t *scope, const gint32 time_val)
{
	wmem_strbuf_t *buf;

	if (time_val == 0) {
		return wmem_strdup(scope, "0 seconds");
	}

	buf = wmem_strbuf_sized_new(scope, TIME_SECS_LEN+1, TIME_SECS_LEN+1);

	signed_time_secs_to_str_buf(time_val, 0, FALSE, buf);

	return wmem_strbuf_finalize(buf);
}