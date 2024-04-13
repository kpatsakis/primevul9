static void timelib_time_reset_fields(timelib_time *time)
{
	assert(time != NULL);

	time->y = 1970;
	time->m = 1;
	time->d = 1;
	time->h = time->i = time->s = 0;
	time->f = 0.0;
	time->tz_info = NULL;
}