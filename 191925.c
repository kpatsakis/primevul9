int4_to_char(PG_FUNCTION_ARGS)
{
	int32		value = PG_GETARG_INT32(0);
	text	   *fmt = PG_GETARG_TEXT_P(1);
	NUMDesc		Num;
	FormatNode *format;
	text	   *result;
	bool		shouldFree;
	int			out_pre_spaces = 0,
				sign = 0;
	char	   *numstr,
			   *orgnum;

	NUM_TOCHAR_prepare;

	/*
	 * On DateType depend part (int32)
	 */
	if (IS_ROMAN(&Num))
		numstr = orgnum = int_to_roman(value);
	else if (IS_EEEE(&Num))
	{
		/* we can do it easily because float8 won't lose any precision */
		float8		val = (float8) value;

		orgnum = (char *) palloc(MAXDOUBLEWIDTH + 1);
		snprintf(orgnum, MAXDOUBLEWIDTH + 1, "%+.*e", Num.post, val);

		/*
		 * Swap a leading positive sign for a space.
		 */
		if (*orgnum == '+')
			*orgnum = ' ';

		numstr = orgnum;
	}
	else
	{
		int			numstr_pre_len;

		if (IS_MULTI(&Num))
		{
			orgnum = DatumGetCString(DirectFunctionCall1(int4out,
														 Int32GetDatum(value * ((int32) pow((double) 10, (double) Num.multi)))));
			Num.pre += Num.multi;
		}
		else
		{
			orgnum = DatumGetCString(DirectFunctionCall1(int4out,
													  Int32GetDatum(value)));
		}

		if (*orgnum == '-')
		{
			sign = '-';
			orgnum++;
		}
		else
			sign = '+';

		numstr_pre_len = strlen(orgnum);

		/* post-decimal digits?  Pad out with zeros. */
		if (Num.post)
		{
			numstr = (char *) palloc(numstr_pre_len + Num.post + 2);
			strcpy(numstr, orgnum);
			*(numstr + numstr_pre_len) = '.';
			memset(numstr + numstr_pre_len + 1, '0', Num.post);
			*(numstr + numstr_pre_len + Num.post + 1) = '\0';
		}
		else
			numstr = orgnum;

		/* needs padding? */
		if (numstr_pre_len < Num.pre)
			out_pre_spaces = Num.pre - numstr_pre_len;
		/* overflowed prefix digit format? */
		else if (numstr_pre_len > Num.pre)
		{
			numstr = (char *) palloc(Num.pre + Num.post + 2);
			fill_str(numstr, '#', Num.pre + Num.post + 1);
			*(numstr + Num.pre) = '.';
		}
	}

	NUM_TOCHAR_finish;
	PG_RETURN_TEXT_P(result);
}