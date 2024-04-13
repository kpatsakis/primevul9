static void timelib_set_relative(char **ptr, timelib_sll amount, int behavior, Scanner *s)
{
	const timelib_relunit* relunit;

	if (!(relunit = timelib_lookup_relunit(ptr))) {
		return;
	}

	switch (relunit->unit) {
		case TIMELIB_MICROSEC: s->time->relative.f += (((double) amount * (double) relunit->multiplier) / 1000000); break;
		case TIMELIB_SECOND:   s->time->relative.s += amount * relunit->multiplier; break;
		case TIMELIB_MINUTE:   s->time->relative.i += amount * relunit->multiplier; break;
		case TIMELIB_HOUR:     s->time->relative.h += amount * relunit->multiplier; break;
		case TIMELIB_DAY:      s->time->relative.d += amount * relunit->multiplier; break;
		case TIMELIB_MONTH:    s->time->relative.m += amount * relunit->multiplier; break;
		case TIMELIB_YEAR:     s->time->relative.y += amount * relunit->multiplier; break;

		case TIMELIB_WEEKDAY:
			TIMELIB_HAVE_WEEKDAY_RELATIVE();
			TIMELIB_UNHAVE_TIME();
			s->time->relative.d += (amount > 0 ? amount - 1 : amount) * 7;
			s->time->relative.weekday = relunit->multiplier;
			s->time->relative.weekday_behavior = behavior;
			break;

		case TIMELIB_SPECIAL:
			TIMELIB_HAVE_SPECIAL_RELATIVE();
			TIMELIB_UNHAVE_TIME();
			s->time->relative.special.type = relunit->multiplier;
			s->time->relative.special.amount = amount;
	}
}