ves_icall_System_CurrentSystemTimeZone_GetTimeZoneData (guint32 year, MonoArray **data, MonoArray **names)
{
#ifndef PLATFORM_WIN32
	MonoDomain *domain = mono_domain_get ();
	struct tm start, tt;
	time_t t;

	long int gmtoff;
	int is_daylight = 0, day;
	char tzone [64];

	MONO_ARCH_SAVE_REGS;

	MONO_CHECK_ARG_NULL (data);
	MONO_CHECK_ARG_NULL (names);

	mono_gc_wbarrier_generic_store (data, (MonoObject*) mono_array_new (domain, mono_defaults.int64_class, 4));
	mono_gc_wbarrier_generic_store (names, (MonoObject*) mono_array_new (domain, mono_defaults.string_class, 2));

	/* 
	 * no info is better than crashing: we'll need our own tz data
	 * to make this work properly, anyway. The range is probably
	 * reduced to 1970 .. 2037 because that is what mktime is
	 * guaranteed to support (we get into an infinite loop
	 * otherwise).
	 */

	memset (&start, 0, sizeof (start));

	start.tm_mday = 1;
	start.tm_year = year-1900;

	t = mktime (&start);

	if ((year < 1970) || (year > 2037) || (t == -1)) {
		t = time (NULL);
		tt = *localtime (&t);
		strftime (tzone, sizeof (tzone), "%Z", &tt);
		mono_array_setref ((*names), 0, mono_string_new (domain, tzone));
		mono_array_setref ((*names), 1, mono_string_new (domain, tzone));
		return 1;
	}

	gmtoff = gmt_offset (&start, t);

	/* For each day of the year, calculate the tm_gmtoff. */
	for (day = 0; day < 365; day++) {

		t += 3600*24;
		tt = *localtime (&t);

		/* Daylight saving starts or ends here. */
		if (gmt_offset (&tt, t) != gmtoff) {
			struct tm tt1;
			time_t t1;

			/* Try to find the exact hour when daylight saving starts/ends. */
			t1 = t;
			do {
				t1 -= 3600;
				tt1 = *localtime (&t1);
			} while (gmt_offset (&tt1, t1) != gmtoff);

			/* Try to find the exact minute when daylight saving starts/ends. */
			do {
				t1 += 60;
				tt1 = *localtime (&t1);
			} while (gmt_offset (&tt1, t1) == gmtoff);
			t1+=gmtoff;
			strftime (tzone, sizeof (tzone), "%Z", &tt);
			
			/* Write data, if we're already in daylight saving, we're done. */
			if (is_daylight) {
				mono_array_setref ((*names), 0, mono_string_new (domain, tzone));
				mono_array_set ((*data), gint64, 1, ((gint64)t1 + EPOCH_ADJUST) * 10000000L);
				return 1;
			} else {
				mono_array_setref ((*names), 1, mono_string_new (domain, tzone));
				mono_array_set ((*data), gint64, 0, ((gint64)t1 + EPOCH_ADJUST) * 10000000L);
				is_daylight = 1;
			}

			/* This is only set once when we enter daylight saving. */
			mono_array_set ((*data), gint64, 2, (gint64)gmtoff * 10000000L);
			mono_array_set ((*data), gint64, 3, (gint64)(gmt_offset (&tt, t) - gmtoff) * 10000000L);

			gmtoff = gmt_offset (&tt, t);
		}
	}

	if (!is_daylight) {
		strftime (tzone, sizeof (tzone), "%Z", &tt);
		mono_array_setref ((*names), 0, mono_string_new (domain, tzone));
		mono_array_setref ((*names), 1, mono_string_new (domain, tzone));
		mono_array_set ((*data), gint64, 0, 0);
		mono_array_set ((*data), gint64, 1, 0);
		mono_array_set ((*data), gint64, 2, (gint64) gmtoff * 10000000L);
		mono_array_set ((*data), gint64, 3, 0);
	}

	return 1;
#else
	MonoDomain *domain = mono_domain_get ();
	TIME_ZONE_INFORMATION tz_info;
	FILETIME ft;
	int i;
	int err, tz_id;

	tz_id = GetTimeZoneInformation (&tz_info);
	if (tz_id == TIME_ZONE_ID_INVALID)
		return 0;

	MONO_CHECK_ARG_NULL (data);
	MONO_CHECK_ARG_NULL (names);

	mono_gc_wbarrier_generic_store (data, mono_array_new (domain, mono_defaults.int64_class, 4));
	mono_gc_wbarrier_generic_store (names, mono_array_new (domain, mono_defaults.string_class, 2));

	for (i = 0; i < 32; ++i)
		if (!tz_info.DaylightName [i])
			break;
	mono_array_setref ((*names), 1, mono_string_new_utf16 (domain, tz_info.DaylightName, i));
	for (i = 0; i < 32; ++i)
		if (!tz_info.StandardName [i])
			break;
	mono_array_setref ((*names), 0, mono_string_new_utf16 (domain, tz_info.StandardName, i));

	if ((year <= 1601) || (year > 30827)) {
		/*
		 * According to MSDN, the MS time functions can't handle dates outside
		 * this interval.
		 */
		return 1;
	}

	/* even if the timezone has no daylight savings it may have Bias (e.g. GMT+13 it seems) */
	if (tz_id != TIME_ZONE_ID_UNKNOWN) {
		tz_info.StandardDate.wYear = year;
		convert_to_absolute_date(&tz_info.StandardDate);
		err = SystemTimeToFileTime (&tz_info.StandardDate, &ft);
		//g_assert(err);
		if (err == 0)
			return 0;
		
		mono_array_set ((*data), gint64, 1, FILETIME_ADJUST + (((guint64)ft.dwHighDateTime<<32) | ft.dwLowDateTime));
		tz_info.DaylightDate.wYear = year;
		convert_to_absolute_date(&tz_info.DaylightDate);
		err = SystemTimeToFileTime (&tz_info.DaylightDate, &ft);
		//g_assert(err);
		if (err == 0)
			return 0;
		
		mono_array_set ((*data), gint64, 0, FILETIME_ADJUST + (((guint64)ft.dwHighDateTime<<32) | ft.dwLowDateTime));
	}
	mono_array_set ((*data), gint64, 2, (tz_info.Bias + tz_info.StandardBias) * -600000000LL);
	mono_array_set ((*data), gint64, 3, (tz_info.DaylightBias - tz_info.StandardBias) * -600000000LL);

	return 1;
#endif
}