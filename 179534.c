signed_time_msecs_to_str(wmem_allocator_t *scope, gint32 time_val)
{
	wmem_strbuf_t *buf;
	int msecs;

	if (time_val == 0) {
		return wmem_strdup(scope, "0 seconds");
	}

	buf = wmem_strbuf_sized_new(scope, TIME_SECS_LEN+1+3+1, TIME_SECS_LEN+1+3+1);

	if (time_val<0) {
		/* oops we got passed a negative time */
		time_val= -time_val;
		msecs = time_val % 1000;
		time_val /= 1000;
		time_val= -time_val;
	} else {
		msecs = time_val % 1000;
		time_val /= 1000;
	}

	signed_time_secs_to_str_buf(time_val, msecs, FALSE, buf);

	return wmem_strbuf_finalize(buf);
}