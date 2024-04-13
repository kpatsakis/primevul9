from_char_set_mode(TmFromChar *tmfc, const FromCharDateMode mode)
{
	if (mode != FROM_CHAR_DATE_NONE)
	{
		if (tmfc->mode == FROM_CHAR_DATE_NONE)
			tmfc->mode = mode;
		else if (tmfc->mode != mode)
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_DATETIME_FORMAT),
					 errmsg("invalid combination of date conventions"),
					 errhint("Do not mix Gregorian and ISO week date "
							 "conventions in a formatting template.")));
	}
}