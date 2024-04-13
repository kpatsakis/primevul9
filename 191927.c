do_to_timestamp(text *date_txt, text *fmt,
				struct pg_tm * tm, fsec_t *fsec)
{
	FormatNode *format;
	TmFromChar	tmfc;
	int			fmt_len;

	ZERO_tmfc(&tmfc);
	ZERO_tm(tm);
	*fsec = 0;

	fmt_len = VARSIZE_ANY_EXHDR(fmt);

	if (fmt_len)
	{
		char	   *fmt_str;
		char	   *date_str;
		bool		incache;

		fmt_str = text_to_cstring(fmt);

		/*
		 * Allocate new memory if format picture is bigger than static cache
		 * and not use cache (call parser always)
		 */
		if (fmt_len > DCH_CACHE_SIZE)
		{
			format = (FormatNode *) palloc((fmt_len + 1) * sizeof(FormatNode));
			incache = FALSE;

			parse_format(format, fmt_str, DCH_keywords,
						 DCH_suff, DCH_index, DCH_TYPE, NULL);

			(format + fmt_len)->type = NODE_TYPE_END;	/* Paranoia? */
		}
		else
		{
			/*
			 * Use cache buffers
			 */
			DCHCacheEntry *ent;

			incache = TRUE;

			if ((ent = DCH_cache_search(fmt_str)) == NULL)
			{
				ent = DCH_cache_getnew(fmt_str);

				/*
				 * Not in the cache, must run parser and save a new
				 * format-picture to the cache.
				 */
				parse_format(ent->format, fmt_str, DCH_keywords,
							 DCH_suff, DCH_index, DCH_TYPE, NULL);

				(ent->format + fmt_len)->type = NODE_TYPE_END;	/* Paranoia? */
#ifdef DEBUG_TO_FROM_CHAR
				/* dump_node(ent->format, fmt_len); */
				/* dump_index(DCH_keywords, DCH_index); */
#endif
			}
			format = ent->format;
		}

#ifdef DEBUG_TO_FROM_CHAR
		/* dump_node(format, fmt_len); */
#endif

		date_str = text_to_cstring(date_txt);

		DCH_from_char(format, date_str, &tmfc);

		pfree(date_str);
		pfree(fmt_str);
		if (!incache)
			pfree(format);
	}

	DEBUG_TMFC(&tmfc);

	/*
	 * Convert values that user define for FROM_CHAR (to_date/to_timestamp) to
	 * standard 'tm'
	 */
	if (tmfc.ssss)
	{
		int			x = tmfc.ssss;

		tm->tm_hour = x / SECS_PER_HOUR;
		x %= SECS_PER_HOUR;
		tm->tm_min = x / SECS_PER_MINUTE;
		x %= SECS_PER_MINUTE;
		tm->tm_sec = x;
	}

	if (tmfc.ss)
		tm->tm_sec = tmfc.ss;
	if (tmfc.mi)
		tm->tm_min = tmfc.mi;
	if (tmfc.hh)
		tm->tm_hour = tmfc.hh;

	if (tmfc.clock == CLOCK_12_HOUR)
	{
		if (tm->tm_hour < 1 || tm->tm_hour > HOURS_PER_DAY / 2)
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_DATETIME_FORMAT),
					 errmsg("hour \"%d\" is invalid for the 12-hour clock",
							tm->tm_hour),
					 errhint("Use the 24-hour clock, or give an hour between 1 and 12.")));

		if (tmfc.pm && tm->tm_hour < HOURS_PER_DAY / 2)
			tm->tm_hour += HOURS_PER_DAY / 2;
		else if (!tmfc.pm && tm->tm_hour == HOURS_PER_DAY / 2)
			tm->tm_hour = 0;
	}

	if (tmfc.year)
	{
		/*
		 * If CC and YY (or Y) are provided, use YY as 2 low-order digits for
		 * the year in the given century.  Keep in mind that the 21st century
		 * AD runs from 2001-2100, not 2000-2099; 6th century BC runs from
		 * 600BC to 501BC.
		 */
		if (tmfc.cc && tmfc.yysz <= 2)
		{
			if (tmfc.bc)
				tmfc.cc = -tmfc.cc;
			tm->tm_year = tmfc.year % 100;
			if (tm->tm_year)
			{
				if (tmfc.cc >= 0)
					tm->tm_year += (tmfc.cc - 1) * 100;
				else
					tm->tm_year = (tmfc.cc + 1) * 100 - tm->tm_year + 1;
			}
			else
				/* find century year for dates ending in "00" */
				tm->tm_year = tmfc.cc * 100 + ((tmfc.cc >= 0) ? 0 : 1);
		}
		else
			/* If a 4-digit year is provided, we use that and ignore CC. */
		{
			tm->tm_year = tmfc.year;
			if (tmfc.bc && tm->tm_year > 0)
				tm->tm_year = -(tm->tm_year - 1);
		}
	}
	else if (tmfc.cc)			/* use first year of century */
	{
		if (tmfc.bc)
			tmfc.cc = -tmfc.cc;
		if (tmfc.cc >= 0)
			/* +1 because 21st century started in 2001 */
			tm->tm_year = (tmfc.cc - 1) * 100 + 1;
		else
			/* +1 because year == 599 is 600 BC */
			tm->tm_year = tmfc.cc * 100 + 1;
	}

	if (tmfc.j)
		j2date(tmfc.j, &tm->tm_year, &tm->tm_mon, &tm->tm_mday);

	if (tmfc.ww)
	{
		if (tmfc.mode == FROM_CHAR_DATE_ISOWEEK)
		{
			/*
			 * If tmfc.d is not set, then the date is left at the beginning of
			 * the ISO week (Monday).
			 */
			if (tmfc.d)
				isoweekdate2date(tmfc.ww, tmfc.d, &tm->tm_year, &tm->tm_mon, &tm->tm_mday);
			else
				isoweek2date(tmfc.ww, &tm->tm_year, &tm->tm_mon, &tm->tm_mday);
		}
		else
			tmfc.ddd = (tmfc.ww - 1) * 7 + 1;
	}

	if (tmfc.w)
		tmfc.dd = (tmfc.w - 1) * 7 + 1;
	if (tmfc.d)
		tm->tm_wday = tmfc.d - 1;		/* convert to native numbering */
	if (tmfc.dd)
		tm->tm_mday = tmfc.dd;
	if (tmfc.ddd)
		tm->tm_yday = tmfc.ddd;
	if (tmfc.mm)
		tm->tm_mon = tmfc.mm;

	if (tmfc.ddd && (tm->tm_mon <= 1 || tm->tm_mday <= 1))
	{
		/*
		 * The month and day field have not been set, so we use the
		 * day-of-year field to populate them.  Depending on the date mode,
		 * this field may be interpreted as a Gregorian day-of-year, or an ISO
		 * week date day-of-year.
		 */

		if (!tm->tm_year && !tmfc.bc)
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_DATETIME_FORMAT),
			errmsg("cannot calculate day of year without year information")));

		if (tmfc.mode == FROM_CHAR_DATE_ISOWEEK)
		{
			int			j0;		/* zeroth day of the ISO year, in Julian */

			j0 = isoweek2j(tm->tm_year, 1) - 1;

			j2date(j0 + tmfc.ddd, &tm->tm_year, &tm->tm_mon, &tm->tm_mday);
		}
		else
		{
			const int  *y;
			int			i;

			static const int ysum[2][13] = {
				{0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365},
			{0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366}};

			y = ysum[isleap(tm->tm_year)];

			for (i = 1; i <= MONTHS_PER_YEAR; i++)
			{
				if (tmfc.ddd < y[i])
					break;
			}
			if (tm->tm_mon <= 1)
				tm->tm_mon = i;

			if (tm->tm_mday <= 1)
				tm->tm_mday = tmfc.ddd - y[i - 1];
		}
	}

#ifdef HAVE_INT64_TIMESTAMP
	if (tmfc.ms)
		*fsec += tmfc.ms * 1000;
	if (tmfc.us)
		*fsec += tmfc.us;
#else
	if (tmfc.ms)
		*fsec += (double) tmfc.ms / 1000;
	if (tmfc.us)
		*fsec += (double) tmfc.us / 1000000;
#endif

	DEBUG_TM(tm);
}