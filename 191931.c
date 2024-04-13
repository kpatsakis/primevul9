numeric_to_number(PG_FUNCTION_ARGS)
{
	text	   *value = PG_GETARG_TEXT_P(0);
	text	   *fmt = PG_GETARG_TEXT_P(1);
	NUMDesc		Num;
	Datum		result;
	FormatNode *format;
	char	   *numstr;
	bool		shouldFree;
	int			len = 0;
	int			scale,
				precision;

	len = VARSIZE(fmt) - VARHDRSZ;

	if (len <= 0 || len >= INT_MAX / NUM_MAX_ITEM_SIZ)
		PG_RETURN_NULL();

	format = NUM_cache(len, &Num, fmt, &shouldFree);

	numstr = (char *) palloc((len * NUM_MAX_ITEM_SIZ) + 1);

	NUM_processor(format, &Num, VARDATA(value), numstr,
				  VARSIZE(value) - VARHDRSZ, 0, 0, false, PG_GET_COLLATION());

	scale = Num.post;
	precision = Max(0, Num.pre) + scale;

	if (shouldFree)
		pfree(format);

	result = DirectFunctionCall3(numeric_in,
								 CStringGetDatum(numstr),
								 ObjectIdGetDatum(InvalidOid),
					  Int32GetDatum(((precision << 16) | scale) + VARHDRSZ));
	pfree(numstr);
	return result;
}