timelib_time *timelib_parse_from_format(char *format, char *string, size_t len, timelib_error_container **errors, const timelib_tzdb *tzdb, timelib_tz_get_wrapper tz_get_wrapper)
{
	char       *fptr = format;
	char       *ptr = string;
	char       *begin;
	timelib_sll tmp;
	Scanner in;
	Scanner *s = &in;
	int allow_extra = 0;

	memset(&in, 0, sizeof(in));
	in.errors = timelib_malloc(sizeof(struct timelib_error_container));
	in.errors->warning_count = 0;
	in.errors->warning_messages = NULL;
	in.errors->error_count = 0;
	in.errors->error_messages = NULL;

	in.time = timelib_time_ctor();
	in.time->y = TIMELIB_UNSET;
	in.time->d = TIMELIB_UNSET;
	in.time->m = TIMELIB_UNSET;
	in.time->h = TIMELIB_UNSET;
	in.time->i = TIMELIB_UNSET;
	in.time->s = TIMELIB_UNSET;
	in.time->f = TIMELIB_UNSET;
	in.time->z = TIMELIB_UNSET;
	in.time->dst = TIMELIB_UNSET;
	in.tzdb = tzdb;
	in.time->is_localtime = 0;
	in.time->zone_type = 0;

	/* Loop over the format string */
	while (*fptr && *ptr) {
		begin = ptr;
		switch (*fptr) {
			case 'D': /* three letter day */
			case 'l': /* full day */
				{
					const timelib_relunit* tmprel = 0;

					tmprel = timelib_lookup_relunit((char **) &ptr);
					if (!tmprel) {
						add_pbf_error(s, "A textual day could not be found", string, begin);
						break;
					} else {
						in.time->have_relative = 1;
						in.time->relative.have_weekday_relative = 1;
						in.time->relative.weekday = tmprel->multiplier;
						in.time->relative.weekday_behavior = 1;
					}
				}
				break;
			case 'd': /* two digit day, with leading zero */
			case 'j': /* two digit day, without leading zero */
				TIMELIB_CHECK_NUMBER;
				if ((s->time->d = timelib_get_nr((char **) &ptr, 2)) == TIMELIB_UNSET) {
					add_pbf_error(s, "A two digit day could not be found", string, begin);
				}
				break;
			case 'S': /* day suffix, ignored, nor checked */
				timelib_skip_day_suffix((char **) &ptr);
				break;
			case 'z': /* day of year - resets month (0 based) - also initializes everything else to !TIMELIB_UNSET */
				TIMELIB_CHECK_NUMBER;
				if ((tmp = timelib_get_nr((char **) &ptr, 3)) == TIMELIB_UNSET) {
					add_pbf_error(s, "A three digit day-of-year could not be found", string, begin);
				} else {
					s->time->m = 1;
					s->time->d = tmp + 1;
					timelib_do_normalize(s->time);
				}
				break;

			case 'm': /* two digit month, with leading zero */
			case 'n': /* two digit month, without leading zero */
				TIMELIB_CHECK_NUMBER;
				if ((s->time->m = timelib_get_nr((char **) &ptr, 2)) == TIMELIB_UNSET) {
					add_pbf_error(s, "A two digit month could not be found", string, begin);
				}
				break;
			case 'M': /* three letter month */
			case 'F': /* full month */
				tmp = timelib_lookup_month((char **) &ptr);
				if (!tmp) {
					add_pbf_error(s, "A textual month could not be found", string, begin);
				} else {
					s->time->m = tmp;
				}
				break;
			case 'y': /* two digit year */
				{
					int length = 0;
					TIMELIB_CHECK_NUMBER;
					if ((s->time->y = timelib_get_nr_ex((char **) &ptr, 2, &length)) == TIMELIB_UNSET) {
						add_pbf_error(s, "A two digit year could not be found", string, begin);
					}
					TIMELIB_PROCESS_YEAR(s->time->y, length);
				}
				break;
			case 'Y': /* four digit year */
				TIMELIB_CHECK_NUMBER;
				if ((s->time->y = timelib_get_nr((char **) &ptr, 4)) == TIMELIB_UNSET) {
					add_pbf_error(s, "A four digit year could not be found", string, begin);
				}
				break;
			case 'g': /* two digit hour, with leading zero */
			case 'h': /* two digit hour, without leading zero */
				TIMELIB_CHECK_NUMBER;
				if ((s->time->h = timelib_get_nr((char **) &ptr, 2)) == TIMELIB_UNSET) {
					add_pbf_error(s, "A two digit hour could not be found", string, begin);
				}
				if (s->time->h > 12) {
					add_pbf_error(s, "Hour can not be higher than 12", string, begin);
				}
				break;
			case 'G': /* two digit hour, with leading zero */
			case 'H': /* two digit hour, without leading zero */
				TIMELIB_CHECK_NUMBER;
				if ((s->time->h = timelib_get_nr((char **) &ptr, 2)) == TIMELIB_UNSET) {
					add_pbf_error(s, "A two digit hour could not be found", string, begin);
				}
				break;
			case 'a': /* am/pm/a.m./p.m. */
			case 'A': /* AM/PM/A.M./P.M. */
				if (s->time->h == TIMELIB_UNSET) {
					add_pbf_error(s, "Meridian can only come after an hour has been found", string, begin);
				} else if ((tmp = timelib_meridian_with_check((char **) &ptr, s->time->h)) == TIMELIB_UNSET) {
					add_pbf_error(s, "A meridian could not be found", string, begin);
				} else {
					s->time->h += tmp;
				}
				break;
			case 'i': /* two digit minute, with leading zero */
				{
					int length;
					timelib_sll min;

					TIMELIB_CHECK_NUMBER;
					min = timelib_get_nr_ex((char **) &ptr, 2, &length);
					if (min == TIMELIB_UNSET || length != 2) {
						add_pbf_error(s, "A two digit minute could not be found", string, begin);
					} else {
						s->time->i = min;
					}
				}
				break;
			case 's': /* two digit second, with leading zero */
				{
					int length;
					timelib_sll sec;

					TIMELIB_CHECK_NUMBER;
					sec = timelib_get_nr_ex((char **) &ptr, 2, &length);
					if (sec == TIMELIB_UNSET || length != 2) {
						add_pbf_error(s, "A two digit second could not be found", string, begin);
					} else {
						s->time->s = sec;
					}
				}
				break;
			case 'u': /* up to six digit millisecond */
				{
					double f;
					char *tptr;

					TIMELIB_CHECK_NUMBER;
					tptr = ptr;
					if ((f = timelib_get_nr((char **) &ptr, 6)) == TIMELIB_UNSET || (ptr - tptr < 1)) {
						add_pbf_error(s, "A six digit millisecond could not be found", string, begin);
					} else {
						s->time->f = (f / pow(10, (ptr - tptr)));
					}
				}
				break;
			case ' ': /* any sort of whitespace (' ' and \t) */
				timelib_eat_spaces((char **) &ptr);
				break;
			case 'U': /* epoch seconds */
				TIMELIB_CHECK_SIGNED_NUMBER;
				TIMELIB_HAVE_RELATIVE();
				tmp = timelib_get_unsigned_nr((char **) &ptr, 24);
				s->time->y = 1970;
				s->time->m = 1;
				s->time->d = 1;
				s->time->h = s->time->i = s->time->s = 0;
				s->time->relative.s += tmp;
				s->time->is_localtime = 1;
				s->time->zone_type = TIMELIB_ZONETYPE_OFFSET;
				s->time->z = 0;
				s->time->dst = 0;
				break;

			case 'e': /* timezone */
			case 'P': /* timezone */
			case 'T': /* timezone */
			case 'O': /* timezone */
				{
					int tz_not_found;
					s->time->z = timelib_parse_zone((char **) &ptr, &s->time->dst, s->time, &tz_not_found, s->tzdb, tz_get_wrapper);
					if (tz_not_found) {
						add_pbf_error(s, "The timezone could not be found in the database", string, begin);
					}
				}
				break;

			case '#': /* separation symbol */
				if (*ptr == ';' || *ptr == ':' || *ptr == '/' || *ptr == '.' || *ptr == ',' || *ptr == '-' || *ptr == '(' || *ptr == ')') {
					++ptr;
				} else {
					add_pbf_error(s, "The separation symbol ([;:/.,-]) could not be found", string, begin);
				}
				break;

			case ';':
			case ':':
			case '/':
			case '.':
			case ',':
			case '-':
			case '(':
			case ')':
				if (*ptr == *fptr) {
					++ptr;
				} else {
					add_pbf_error(s, "The separation symbol could not be found", string, begin);
				}
				break;

			case '!': /* reset all fields to default */
				timelib_time_reset_fields(s->time);
				break; /* break intentionally not missing */

			case '|': /* reset all fields to default when not set */
				timelib_time_reset_unset_fields(s->time);
				break; /* break intentionally not missing */

			case '?': /* random char */
				++ptr;
				break;

			case '\\': /* escaped char */
				if(!fptr[1]) {
					add_pbf_error(s, "Escaped character expected", string, begin);
					break;
				}
				fptr++;
				if (*ptr == *fptr) {
					++ptr;
				} else {
					add_pbf_error(s, "The escaped character could not be found", string, begin);
				}
				break;

			case '*': /* random chars until a separator or number ([ \t.,:;/-0123456789]) */
				timelib_eat_until_separator((char **) &ptr);
				break;

			case '+': /* allow extra chars in the format */
				allow_extra = 1;
				break;

			default:
				if (*fptr != *ptr) {
					add_pbf_error(s, "The format separator does not match", string, begin);
				}
				ptr++;
		}
		fptr++;
	}
	if (*ptr) {
		if (allow_extra) {
			add_pbf_warning(s, "Trailing data", string, ptr);
		} else {
			add_pbf_error(s, "Trailing data", string, ptr);
		}
	}
	/* ignore trailing +'s */
	while (*fptr == '+') {
		fptr++;
	}
	if (*fptr) {
		/* Trailing | and ! specifiers are valid. */
		int done = 0;
		while (*fptr && !done) {
			switch (*fptr++) {
				case '!': /* reset all fields to default */
					timelib_time_reset_fields(s->time);
					break;

				case '|': /* reset all fields to default when not set */
					timelib_time_reset_unset_fields(s->time);
					break;

				default:
					add_pbf_error(s, "Data missing", string, ptr);
					done = 1;
			}
		}
	}

	/* clean up a bit */
	if (s->time->h != TIMELIB_UNSET || s->time->i != TIMELIB_UNSET || s->time->s != TIMELIB_UNSET) {
		if (s->time->h == TIMELIB_UNSET ) {
			s->time->h = 0;
		}
		if (s->time->i == TIMELIB_UNSET ) {
			s->time->i = 0;
		}
		if (s->time->s == TIMELIB_UNSET ) {
			s->time->s = 0;
		}
	}

	/* do funky checking whether the parsed time was valid time */
	if (s->time->h != TIMELIB_UNSET && s->time->i != TIMELIB_UNSET &&
		s->time->s != TIMELIB_UNSET &&
		!timelib_valid_time( s->time->h, s->time->i, s->time->s)) {
		add_pbf_warning(s, "The parsed time was invalid", string, ptr);
	}
	/* do funky checking whether the parsed date was valid date */
	if (s->time->y != TIMELIB_UNSET && s->time->m != TIMELIB_UNSET &&
		s->time->d != TIMELIB_UNSET &&
		!timelib_valid_date( s->time->y, s->time->m, s->time->d)) {
		add_pbf_warning(s, "The parsed date was invalid", string, ptr);
	}

	if (errors) {
		*errors = in.errors;
	} else {
		timelib_error_container_dtor(in.errors);
	}
	return in.time;
}