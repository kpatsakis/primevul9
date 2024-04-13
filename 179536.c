rel_time_to_str(wmem_allocator_t *scope, const nstime_t *rel_time)
{
	wmem_strbuf_t *buf;
	gint32 time_val;
	gint32 nsec;

	/* If the nanoseconds part of the time stamp is negative,
	   print its absolute value and, if the seconds part isn't
	   (the seconds part should be zero in that case), stick
	   a "-" in front of the entire time stamp. */
	time_val = (gint) rel_time->secs;
	nsec = rel_time->nsecs;
	if (time_val == 0 && nsec == 0) {
		return wmem_strdup(scope, "0.000000000 seconds");
	}

	buf = wmem_strbuf_sized_new(scope, 1+TIME_SECS_LEN+1+6+1, 1+TIME_SECS_LEN+1+6+1);

	if (nsec < 0) {
		nsec = -nsec;
		wmem_strbuf_append_c(buf, '-');

		/*
		 * We assume here that "rel_time->secs" is negative
		 * or zero; if it's not, the time stamp is bogus,
		 * with a positive seconds and negative microseconds.
		 */
		time_val = (gint) -rel_time->secs;
	}

	signed_time_secs_to_str_buf(time_val, nsec, TRUE, buf);

	return wmem_strbuf_finalize(buf);
}