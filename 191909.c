numeric_to_char(PG_FUNCTION_ARGS)
{
	Numeric		value = PG_GETARG_NUMERIC(0);
	text	   *fmt = PG_GETARG_TEXT_P(1);
	NUMDesc		Num;
	FormatNode *format;
	text	   *result;
	bool		shouldFree;
	int			out_pre_spaces = 0,
				sign = 0;
	char	   *numstr,
			   *orgnum,
			   *p;
	Numeric		x;

	NUM_TOCHAR_prepare;

	/*
	 * On DateType depend part (numeric)
	 */
	if (IS_ROMAN(&Num))
	{
		x = DatumGetNumeric(DirectFunctionCall2(numeric_round,
												NumericGetDatum(value),
												Int32GetDatum(0)));
		numstr = orgnum =
			int_to_roman(DatumGetInt32(DirectFunctionCall1(numeric_int4,
													   NumericGetDatum(x))));
	}
	else if (IS_EEEE(&Num))
	{
		orgnum = numeric_out_sci(value, Num.post);

		/*
		 * numeric_out_sci() does not emit a sign for positive numbers.  We
		 * need to add a space in this case so that positive and negative
		 * numbers are aligned.  We also have to do the right thing for NaN.
		 */
		if (strcmp(orgnum, "NaN") == 0)
		{
			/*
			 * Allow 6 characters for the leading sign, the decimal point,
			 * "e", the exponent's sign and two exponent digits.
			 */
			numstr = (char *) palloc(Num.pre + Num.post + 7);
			fill_str(numstr, '#', Num.pre + Num.post + 6);
			*numstr = ' ';
			*(numstr + Num.pre + 1) = '.';
		}
		else if (*orgnum != '-')
		{
			numstr = (char *) palloc(strlen(orgnum) + 2);
			*numstr = ' ';
			strcpy(numstr + 1, orgnum);
		}
		else
		{
			numstr = orgnum;
		}
	}
	else
	{
		int			numstr_pre_len;
		Numeric		val = value;

		if (IS_MULTI(&Num))
		{
			Numeric		a = DatumGetNumeric(DirectFunctionCall1(int4_numeric,
														 Int32GetDatum(10)));
			Numeric		b = DatumGetNumeric(DirectFunctionCall1(int4_numeric,
												  Int32GetDatum(Num.multi)));

			x = DatumGetNumeric(DirectFunctionCall2(numeric_power,
													NumericGetDatum(a),
													NumericGetDatum(b)));
			val = DatumGetNumeric(DirectFunctionCall2(numeric_mul,
													  NumericGetDatum(value),
													  NumericGetDatum(x)));
			Num.pre += Num.multi;
		}

		x = DatumGetNumeric(DirectFunctionCall2(numeric_round,
												NumericGetDatum(val),
												Int32GetDatum(Num.post)));
		orgnum = DatumGetCString(DirectFunctionCall1(numeric_out,
													 NumericGetDatum(x)));

		if (*orgnum == '-')
		{
			sign = '-';
			numstr = orgnum + 1;
		}
		else
		{
			sign = '+';
			numstr = orgnum;
		}

		if ((p = strchr(numstr, '.')))
			numstr_pre_len = p - numstr;
		else
			numstr_pre_len = strlen(numstr);

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