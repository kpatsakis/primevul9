DCH_from_char(FormatNode *node, char *in, TmFromChar *out)
{
	FormatNode *n;
	char	   *s;
	int			len,
				value;
	bool		fx_mode = false;

	for (n = node, s = in; n->type != NODE_TYPE_END && *s != '\0'; n++)
	{
		if (n->type != NODE_TYPE_ACTION)
		{
			/*
			 * Separator, so consume one character from input string.  Notice
			 * we don't insist that the consumed character match the format's
			 * character.
			 */
			s++;
			continue;
		}

		/* Ignore spaces before fields when not in FX (fixed width) mode */
		if (!fx_mode && n->key->id != DCH_FX)
		{
			while (*s != '\0' && isspace((unsigned char) *s))
				s++;
		}

		from_char_set_mode(out, n->key->date_mode);

		switch (n->key->id)
		{
			case DCH_FX:
				fx_mode = true;
				break;
			case DCH_A_M:
			case DCH_P_M:
			case DCH_a_m:
			case DCH_p_m:
				from_char_seq_search(&value, &s, ampm_strings_long,
									 ALL_UPPER, n->key->len, n);
				from_char_set_int(&out->pm, value % 2, n);
				out->clock = CLOCK_12_HOUR;
				break;
			case DCH_AM:
			case DCH_PM:
			case DCH_am:
			case DCH_pm:
				from_char_seq_search(&value, &s, ampm_strings,
									 ALL_UPPER, n->key->len, n);
				from_char_set_int(&out->pm, value % 2, n);
				out->clock = CLOCK_12_HOUR;
				break;
			case DCH_HH:
			case DCH_HH12:
				from_char_parse_int_len(&out->hh, &s, 2, n);
				out->clock = CLOCK_12_HOUR;
				s += SKIP_THth(n->suffix);
				break;
			case DCH_HH24:
				from_char_parse_int_len(&out->hh, &s, 2, n);
				s += SKIP_THth(n->suffix);
				break;
			case DCH_MI:
				from_char_parse_int(&out->mi, &s, n);
				s += SKIP_THth(n->suffix);
				break;
			case DCH_SS:
				from_char_parse_int(&out->ss, &s, n);
				s += SKIP_THth(n->suffix);
				break;
			case DCH_MS:		/* millisecond */
				len = from_char_parse_int_len(&out->ms, &s, 3, n);

				/*
				 * 25 is 0.25 and 250 is 0.25 too; 025 is 0.025 and not 0.25
				 */
				out->ms *= len == 1 ? 100 :
					len == 2 ? 10 : 1;

				s += SKIP_THth(n->suffix);
				break;
			case DCH_US:		/* microsecond */
				len = from_char_parse_int_len(&out->us, &s, 6, n);

				out->us *= len == 1 ? 100000 :
					len == 2 ? 10000 :
					len == 3 ? 1000 :
					len == 4 ? 100 :
					len == 5 ? 10 : 1;

				s += SKIP_THth(n->suffix);
				break;
			case DCH_SSSS:
				from_char_parse_int(&out->ssss, &s, n);
				s += SKIP_THth(n->suffix);
				break;
			case DCH_tz:
			case DCH_TZ:
			case DCH_OF:
				ereport(ERROR,
						(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
						 errmsg("\"TZ\"/\"tz\"/\"OF\" format patterns are not supported in to_date")));
			case DCH_A_D:
			case DCH_B_C:
			case DCH_a_d:
			case DCH_b_c:
				from_char_seq_search(&value, &s, adbc_strings_long,
									 ALL_UPPER, n->key->len, n);
				from_char_set_int(&out->bc, value % 2, n);
				break;
			case DCH_AD:
			case DCH_BC:
			case DCH_ad:
			case DCH_bc:
				from_char_seq_search(&value, &s, adbc_strings,
									 ALL_UPPER, n->key->len, n);
				from_char_set_int(&out->bc, value % 2, n);
				break;
			case DCH_MONTH:
			case DCH_Month:
			case DCH_month:
				from_char_seq_search(&value, &s, months_full, ONE_UPPER,
									 MAX_MONTH_LEN, n);
				from_char_set_int(&out->mm, value + 1, n);
				break;
			case DCH_MON:
			case DCH_Mon:
			case DCH_mon:
				from_char_seq_search(&value, &s, months, ONE_UPPER,
									 MAX_MON_LEN, n);
				from_char_set_int(&out->mm, value + 1, n);
				break;
			case DCH_MM:
				from_char_parse_int(&out->mm, &s, n);
				s += SKIP_THth(n->suffix);
				break;
			case DCH_DAY:
			case DCH_Day:
			case DCH_day:
				from_char_seq_search(&value, &s, days, ONE_UPPER,
									 MAX_DAY_LEN, n);
				from_char_set_int(&out->d, value, n);
				out->d++;
				break;
			case DCH_DY:
			case DCH_Dy:
			case DCH_dy:
				from_char_seq_search(&value, &s, days, ONE_UPPER,
									 MAX_DY_LEN, n);
				from_char_set_int(&out->d, value, n);
				out->d++;
				break;
			case DCH_DDD:
				from_char_parse_int(&out->ddd, &s, n);
				s += SKIP_THth(n->suffix);
				break;
			case DCH_IDDD:
				from_char_parse_int_len(&out->ddd, &s, 3, n);
				s += SKIP_THth(n->suffix);
				break;
			case DCH_DD:
				from_char_parse_int(&out->dd, &s, n);
				s += SKIP_THth(n->suffix);
				break;
			case DCH_D:
				from_char_parse_int(&out->d, &s, n);
				s += SKIP_THth(n->suffix);
				break;
			case DCH_ID:
				from_char_parse_int_len(&out->d, &s, 1, n);
				/* Shift numbering to match Gregorian where Sunday = 1 */
				if (++out->d > 7)
					out->d = 1;
				s += SKIP_THth(n->suffix);
				break;
			case DCH_WW:
			case DCH_IW:
				from_char_parse_int(&out->ww, &s, n);
				s += SKIP_THth(n->suffix);
				break;
			case DCH_Q:

				/*
				 * We ignore 'Q' when converting to date because it is unclear
				 * which date in the quarter to use, and some people specify
				 * both quarter and month, so if it was honored it might
				 * conflict with the supplied month. That is also why we don't
				 * throw an error.
				 *
				 * We still parse the source string for an integer, but it
				 * isn't stored anywhere in 'out'.
				 */
				from_char_parse_int((int *) NULL, &s, n);
				s += SKIP_THth(n->suffix);
				break;
			case DCH_CC:
				from_char_parse_int(&out->cc, &s, n);
				s += SKIP_THth(n->suffix);
				break;
			case DCH_Y_YYY:
				{
					int			matched,
								years,
								millenia;

					matched = sscanf(s, "%d,%03d", &millenia, &years);
					if (matched != 2)
						ereport(ERROR,
								(errcode(ERRCODE_INVALID_DATETIME_FORMAT),
							  errmsg("invalid input string for \"Y,YYY\"")));
					years += (millenia * 1000);
					from_char_set_int(&out->year, years, n);
					out->yysz = 4;
					s += strdigits_len(s) + 4 + SKIP_THth(n->suffix);
				}
				break;
			case DCH_YYYY:
			case DCH_IYYY:
				from_char_parse_int(&out->year, &s, n);
				out->yysz = 4;
				s += SKIP_THth(n->suffix);
				break;
			case DCH_YYY:
			case DCH_IYY:
				if (from_char_parse_int(&out->year, &s, n) < 4)
					out->year = adjust_partial_year_to_2020(out->year);
				out->yysz = 3;
				s += SKIP_THth(n->suffix);
				break;
			case DCH_YY:
			case DCH_IY:
				if (from_char_parse_int(&out->year, &s, n) < 4)
					out->year = adjust_partial_year_to_2020(out->year);
				out->yysz = 2;
				s += SKIP_THth(n->suffix);
				break;
			case DCH_Y:
			case DCH_I:
				if (from_char_parse_int(&out->year, &s, n) < 4)
					out->year = adjust_partial_year_to_2020(out->year);
				out->yysz = 1;
				s += SKIP_THth(n->suffix);
				break;
			case DCH_RM:
				from_char_seq_search(&value, &s, rm_months_upper,
									 ALL_UPPER, MAX_RM_LEN, n);
				from_char_set_int(&out->mm, MONTHS_PER_YEAR - value, n);
				break;
			case DCH_rm:
				from_char_seq_search(&value, &s, rm_months_lower,
									 ALL_LOWER, MAX_RM_LEN, n);
				from_char_set_int(&out->mm, MONTHS_PER_YEAR - value, n);
				break;
			case DCH_W:
				from_char_parse_int(&out->w, &s, n);
				s += SKIP_THth(n->suffix);
				break;
			case DCH_J:
				from_char_parse_int(&out->j, &s, n);
				s += SKIP_THth(n->suffix);
				break;
		}
	}
}