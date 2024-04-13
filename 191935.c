timestamp_to_char(PG_FUNCTION_ARGS)
{
	Timestamp	dt = PG_GETARG_TIMESTAMP(0);
	text	   *fmt = PG_GETARG_TEXT_P(1),
			   *res;
	TmToChar	tmtc;
	struct pg_tm *tm;
	int			thisdate;

	if ((VARSIZE(fmt) - VARHDRSZ) <= 0 || TIMESTAMP_NOT_FINITE(dt))
		PG_RETURN_NULL();

	ZERO_tmtc(&tmtc);
	tm = tmtcTm(&tmtc);

	if (timestamp2tm(dt, NULL, tm, &tmtcFsec(&tmtc), NULL, NULL) != 0)
		ereport(ERROR,
				(errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
				 errmsg("timestamp out of range")));

	thisdate = date2j(tm->tm_year, tm->tm_mon, tm->tm_mday);
	tm->tm_wday = (thisdate + 1) % 7;
	tm->tm_yday = thisdate - date2j(tm->tm_year, 1, 1) + 1;

	if (!(res = datetime_to_char_body(&tmtc, fmt, false, PG_GET_COLLATION())))
		PG_RETURN_NULL();

	PG_RETURN_TEXT_P(res);
}