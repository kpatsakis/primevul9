to_timestamp(PG_FUNCTION_ARGS)
{
	text	   *date_txt = PG_GETARG_TEXT_P(0);
	text	   *fmt = PG_GETARG_TEXT_P(1);
	Timestamp	result;
	int			tz;
	struct pg_tm tm;
	fsec_t		fsec;

	do_to_timestamp(date_txt, fmt, &tm, &fsec);

	tz = DetermineTimeZoneOffset(&tm, session_timezone);

	if (tm2timestamp(&tm, fsec, &tz, &result) != 0)
		ereport(ERROR,
				(errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
				 errmsg("timestamp out of range")));

	PG_RETURN_TIMESTAMP(result);
}