signed_time_secs_to_str_buf(gint32 time_val, const guint32 frac,
    const gboolean is_nsecs, wmem_strbuf_t *buf)
{
	if(time_val < 0){
		wmem_strbuf_append_printf(buf, "-");
		if(time_val == G_MININT32) {
			/*
			 * You can't fit time_val's absolute value into
			 * a 32-bit signed integer.  Just directly
			 * pass G_MAXUINT32, which is its absolute
			 * value, directly to unsigned_time_secs_to_str_buf().
			 *
			 * (XXX - does ISO C guarantee that -(-2^n),
			 * when calculated and cast to an n-bit unsigned
			 * integer type, will have the value 2^n?)
			 */
			unsigned_time_secs_to_str_buf(G_MAXUINT32, frac,
			    is_nsecs, buf);
		} else {
			/*
			 * We now know -secs will fit into a guint32;
			 * negate it and pass that to
			 * unsigned_time_secs_to_str_buf().
			 */
			unsigned_time_secs_to_str_buf(-time_val, frac,
			    is_nsecs, buf);
		}
	} else
		unsigned_time_secs_to_str_buf(time_val, frac, is_nsecs, buf);
}