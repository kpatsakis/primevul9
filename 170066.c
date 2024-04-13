static void timelib_time_reset_unset_fields(timelib_time *time)
{
	assert(time != NULL);

	if (time->y == TIMELIB_UNSET ) time->y = 1970;
	if (time->m == TIMELIB_UNSET ) time->m = 1;
	if (time->d == TIMELIB_UNSET ) time->d = 1;
	if (time->h == TIMELIB_UNSET ) time->h = 0;
	if (time->i == TIMELIB_UNSET ) time->i = 0;
	if (time->s == TIMELIB_UNSET ) time->s = 0;
	if (time->f == TIMELIB_UNSET ) time->f = 0.0;
}