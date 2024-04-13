interval_to_char(PG_FUNCTION_ARGS)
{
	Interval   *it = PG_GETARG_INTERVAL_P(0);
	text	   *fmt = PG_GETARG_TEXT_P(1),
			   *res;
	TmToChar	tmtc;
	struct pg_tm *tm;

	if ((VARSIZE(fmt) - VARHDRSZ) <= 0)
		PG_RETURN_NULL();

	ZERO_tmtc(&tmtc);
	tm = tmtcTm(&tmtc);

	if (interval2tm(*it, tm, &tmtcFsec(&tmtc)) != 0)
		PG_RETURN_NULL();

	/* wday is meaningless, yday approximates the total span in days */
	tm->tm_yday = (tm->tm_year * MONTHS_PER_YEAR + tm->tm_mon) * DAYS_PER_MONTH + tm->tm_mday;

	if (!(res = datetime_to_char_body(&tmtc, fmt, true, PG_GET_COLLATION())))
		PG_RETURN_NULL();

	PG_RETURN_TEXT_P(res);
}