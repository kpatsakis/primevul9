to_date(PG_FUNCTION_ARGS)
{
	text	   *date_txt = PG_GETARG_TEXT_P(0);
	text	   *fmt = PG_GETARG_TEXT_P(1);
	DateADT		result;
	struct pg_tm tm;
	fsec_t		fsec;

	do_to_timestamp(date_txt, fmt, &tm, &fsec);

	if (!IS_VALID_JULIAN(tm.tm_year, tm.tm_mon, tm.tm_mday))
		ereport(ERROR,
				(errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
				 errmsg("date out of range: \"%s\"",
						text_to_cstring(date_txt))));

	result = date2j(tm.tm_year, tm.tm_mon, tm.tm_mday) - POSTGRES_EPOCH_JDATE;

	PG_RETURN_DATEADT(result);
}