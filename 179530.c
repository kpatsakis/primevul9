rel_time_to_secs_str(wmem_allocator_t *scope, const nstime_t *rel_time)
{
	gchar *buf;

	buf=(gchar *)wmem_alloc(scope, REL_TIME_SECS_LEN);

	display_signed_time(buf, REL_TIME_SECS_LEN, (gint64) rel_time->secs,
			rel_time->nsecs, TO_STR_TIME_RES_T_NSECS);
	return buf;
}