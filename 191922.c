DCH_to_char(FormatNode *node, bool is_interval, TmToChar *in, char *out, Oid collid)
{
	FormatNode *n;
	char	   *s;
	struct pg_tm *tm = &in->tm;
	int			i;

	/* cache localized days and months */
	cache_locale_time();

	s = out;
	for (n = node; n->type != NODE_TYPE_END; n++)
	{
		if (n->type != NODE_TYPE_ACTION)
		{
			*s = n->character;
			s++;
			continue;
		}

		switch (n->key->id)
		{
			case DCH_A_M:
			case DCH_P_M:
				strcpy(s, (tm->tm_hour % HOURS_PER_DAY >= HOURS_PER_DAY / 2)
					   ? P_M_STR : A_M_STR);
				s += strlen(s);
				break;
			case DCH_AM:
			case DCH_PM:
				strcpy(s, (tm->tm_hour % HOURS_PER_DAY >= HOURS_PER_DAY / 2)
					   ? PM_STR : AM_STR);
				s += strlen(s);
				break;
			case DCH_a_m:
			case DCH_p_m:
				strcpy(s, (tm->tm_hour % HOURS_PER_DAY >= HOURS_PER_DAY / 2)
					   ? p_m_STR : a_m_STR);
				s += strlen(s);
				break;
			case DCH_am:
			case DCH_pm:
				strcpy(s, (tm->tm_hour % HOURS_PER_DAY >= HOURS_PER_DAY / 2)
					   ? pm_STR : am_STR);
				s += strlen(s);
				break;
			case DCH_HH:
			case DCH_HH12:

				/*
				 * display time as shown on a 12-hour clock, even for
				 * intervals
				 */
				sprintf(s, "%0*d", S_FM(n->suffix) ? 0 : 2,
				 tm->tm_hour % (HOURS_PER_DAY / 2) == 0 ? HOURS_PER_DAY / 2 :
						tm->tm_hour % (HOURS_PER_DAY / 2));
				if (S_THth(n->suffix))
					str_numth(s, s, S_TH_TYPE(n->suffix));
				s += strlen(s);
				break;
			case DCH_HH24:
				sprintf(s, "%0*d", S_FM(n->suffix) ? 0 : 2, tm->tm_hour);
				if (S_THth(n->suffix))
					str_numth(s, s, S_TH_TYPE(n->suffix));
				s += strlen(s);
				break;
			case DCH_MI:
				sprintf(s, "%0*d", S_FM(n->suffix) ? 0 : 2, tm->tm_min);
				if (S_THth(n->suffix))
					str_numth(s, s, S_TH_TYPE(n->suffix));
				s += strlen(s);
				break;
			case DCH_SS:
				sprintf(s, "%0*d", S_FM(n->suffix) ? 0 : 2, tm->tm_sec);
				if (S_THth(n->suffix))
					str_numth(s, s, S_TH_TYPE(n->suffix));
				s += strlen(s);
				break;
			case DCH_MS:		/* millisecond */
#ifdef HAVE_INT64_TIMESTAMP
				sprintf(s, "%03d", (int) (in->fsec / INT64CONST(1000)));
#else
				/* No rint() because we can't overflow and we might print US */
				sprintf(s, "%03d", (int) (in->fsec * 1000));
#endif
				if (S_THth(n->suffix))
					str_numth(s, s, S_TH_TYPE(n->suffix));
				s += strlen(s);
				break;
			case DCH_US:		/* microsecond */
#ifdef HAVE_INT64_TIMESTAMP
				sprintf(s, "%06d", (int) in->fsec);
#else
				/* don't use rint() because we can't overflow 1000 */
				sprintf(s, "%06d", (int) (in->fsec * 1000000));
#endif
				if (S_THth(n->suffix))
					str_numth(s, s, S_TH_TYPE(n->suffix));
				s += strlen(s);
				break;
			case DCH_SSSS:
				sprintf(s, "%d", tm->tm_hour * SECS_PER_HOUR +
						tm->tm_min * SECS_PER_MINUTE +
						tm->tm_sec);
				if (S_THth(n->suffix))
					str_numth(s, s, S_TH_TYPE(n->suffix));
				s += strlen(s);
				break;
			case DCH_tz:
				INVALID_FOR_INTERVAL;
				if (tmtcTzn(in))
				{
					/* We assume here that timezone names aren't localized */
					char	   *p = asc_tolower_z(tmtcTzn(in));

					strcpy(s, p);
					pfree(p);
					s += strlen(s);
				}
				break;
			case DCH_TZ:
				INVALID_FOR_INTERVAL;
				if (tmtcTzn(in))
				{
					strcpy(s, tmtcTzn(in));
					s += strlen(s);
				}
				break;
			case DCH_OF:
				INVALID_FOR_INTERVAL;
				sprintf(s, "%+0*ld", S_FM(n->suffix) ? 0 : 3, tm->tm_gmtoff / SECS_PER_HOUR);
				s += strlen(s);
				if (tm->tm_gmtoff % SECS_PER_HOUR != 0)
				{
					sprintf(s, ":%02ld", (tm->tm_gmtoff % SECS_PER_HOUR) / SECS_PER_MINUTE);
					s += strlen(s);
				}
				break;
			case DCH_A_D:
			case DCH_B_C:
				INVALID_FOR_INTERVAL;
				strcpy(s, (tm->tm_year <= 0 ? B_C_STR : A_D_STR));
				s += strlen(s);
				break;
			case DCH_AD:
			case DCH_BC:
				INVALID_FOR_INTERVAL;
				strcpy(s, (tm->tm_year <= 0 ? BC_STR : AD_STR));
				s += strlen(s);
				break;
			case DCH_a_d:
			case DCH_b_c:
				INVALID_FOR_INTERVAL;
				strcpy(s, (tm->tm_year <= 0 ? b_c_STR : a_d_STR));
				s += strlen(s);
				break;
			case DCH_ad:
			case DCH_bc:
				INVALID_FOR_INTERVAL;
				strcpy(s, (tm->tm_year <= 0 ? bc_STR : ad_STR));
				s += strlen(s);
				break;
			case DCH_MONTH:
				INVALID_FOR_INTERVAL;
				if (!tm->tm_mon)
					break;
				if (S_TM(n->suffix))
					strcpy(s, str_toupper_z(localized_full_months[tm->tm_mon - 1], collid));
				else
					sprintf(s, "%*s", S_FM(n->suffix) ? 0 : -9,
							asc_toupper_z(months_full[tm->tm_mon - 1]));
				s += strlen(s);
				break;
			case DCH_Month:
				INVALID_FOR_INTERVAL;
				if (!tm->tm_mon)
					break;
				if (S_TM(n->suffix))
					strcpy(s, str_initcap_z(localized_full_months[tm->tm_mon - 1], collid));
				else
					sprintf(s, "%*s", S_FM(n->suffix) ? 0 : -9,
							months_full[tm->tm_mon - 1]);
				s += strlen(s);
				break;
			case DCH_month:
				INVALID_FOR_INTERVAL;
				if (!tm->tm_mon)
					break;
				if (S_TM(n->suffix))
					strcpy(s, str_tolower_z(localized_full_months[tm->tm_mon - 1], collid));
				else
					sprintf(s, "%*s", S_FM(n->suffix) ? 0 : -9,
							asc_tolower_z(months_full[tm->tm_mon - 1]));
				s += strlen(s);
				break;
			case DCH_MON:
				INVALID_FOR_INTERVAL;
				if (!tm->tm_mon)
					break;
				if (S_TM(n->suffix))
					strcpy(s, str_toupper_z(localized_abbrev_months[tm->tm_mon - 1], collid));
				else
					strcpy(s, asc_toupper_z(months[tm->tm_mon - 1]));
				s += strlen(s);
				break;
			case DCH_Mon:
				INVALID_FOR_INTERVAL;
				if (!tm->tm_mon)
					break;
				if (S_TM(n->suffix))
					strcpy(s, str_initcap_z(localized_abbrev_months[tm->tm_mon - 1], collid));
				else
					strcpy(s, months[tm->tm_mon - 1]);
				s += strlen(s);
				break;
			case DCH_mon:
				INVALID_FOR_INTERVAL;
				if (!tm->tm_mon)
					break;
				if (S_TM(n->suffix))
					strcpy(s, str_tolower_z(localized_abbrev_months[tm->tm_mon - 1], collid));
				else
					strcpy(s, asc_tolower_z(months[tm->tm_mon - 1]));
				s += strlen(s);
				break;
			case DCH_MM:
				sprintf(s, "%0*d", S_FM(n->suffix) ? 0 : 2, tm->tm_mon);
				if (S_THth(n->suffix))
					str_numth(s, s, S_TH_TYPE(n->suffix));
				s += strlen(s);
				break;
			case DCH_DAY:
				INVALID_FOR_INTERVAL;
				if (S_TM(n->suffix))
					strcpy(s, str_toupper_z(localized_full_days[tm->tm_wday], collid));
				else
					sprintf(s, "%*s", S_FM(n->suffix) ? 0 : -9,
							asc_toupper_z(days[tm->tm_wday]));
				s += strlen(s);
				break;
			case DCH_Day:
				INVALID_FOR_INTERVAL;
				if (S_TM(n->suffix))
					strcpy(s, str_initcap_z(localized_full_days[tm->tm_wday], collid));
				else
					sprintf(s, "%*s", S_FM(n->suffix) ? 0 : -9,
							days[tm->tm_wday]);
				s += strlen(s);
				break;
			case DCH_day:
				INVALID_FOR_INTERVAL;
				if (S_TM(n->suffix))
					strcpy(s, str_tolower_z(localized_full_days[tm->tm_wday], collid));
				else
					sprintf(s, "%*s", S_FM(n->suffix) ? 0 : -9,
							asc_tolower_z(days[tm->tm_wday]));
				s += strlen(s);
				break;
			case DCH_DY:
				INVALID_FOR_INTERVAL;
				if (S_TM(n->suffix))
					strcpy(s, str_toupper_z(localized_abbrev_days[tm->tm_wday], collid));
				else
					strcpy(s, asc_toupper_z(days_short[tm->tm_wday]));
				s += strlen(s);
				break;
			case DCH_Dy:
				INVALID_FOR_INTERVAL;
				if (S_TM(n->suffix))
					strcpy(s, str_initcap_z(localized_abbrev_days[tm->tm_wday], collid));
				else
					strcpy(s, days_short[tm->tm_wday]);
				s += strlen(s);
				break;
			case DCH_dy:
				INVALID_FOR_INTERVAL;
				if (S_TM(n->suffix))
					strcpy(s, str_tolower_z(localized_abbrev_days[tm->tm_wday], collid));
				else
					strcpy(s, asc_tolower_z(days_short[tm->tm_wday]));
				s += strlen(s);
				break;
			case DCH_DDD:
			case DCH_IDDD:
				sprintf(s, "%0*d", S_FM(n->suffix) ? 0 : 3,
						(n->key->id == DCH_DDD) ?
						tm->tm_yday :
					  date2isoyearday(tm->tm_year, tm->tm_mon, tm->tm_mday));
				if (S_THth(n->suffix))
					str_numth(s, s, S_TH_TYPE(n->suffix));
				s += strlen(s);
				break;
			case DCH_DD:
				sprintf(s, "%0*d", S_FM(n->suffix) ? 0 : 2, tm->tm_mday);
				if (S_THth(n->suffix))
					str_numth(s, s, S_TH_TYPE(n->suffix));
				s += strlen(s);
				break;
			case DCH_D:
				INVALID_FOR_INTERVAL;
				sprintf(s, "%d", tm->tm_wday + 1);
				if (S_THth(n->suffix))
					str_numth(s, s, S_TH_TYPE(n->suffix));
				s += strlen(s);
				break;
			case DCH_ID:
				INVALID_FOR_INTERVAL;
				sprintf(s, "%d", (tm->tm_wday == 0) ? 7 : tm->tm_wday);
				if (S_THth(n->suffix))
					str_numth(s, s, S_TH_TYPE(n->suffix));
				s += strlen(s);
				break;
			case DCH_WW:
				sprintf(s, "%0*d", S_FM(n->suffix) ? 0 : 2,
						(tm->tm_yday - 1) / 7 + 1);
				if (S_THth(n->suffix))
					str_numth(s, s, S_TH_TYPE(n->suffix));
				s += strlen(s);
				break;
			case DCH_IW:
				sprintf(s, "%0*d", S_FM(n->suffix) ? 0 : 2,
						date2isoweek(tm->tm_year, tm->tm_mon, tm->tm_mday));
				if (S_THth(n->suffix))
					str_numth(s, s, S_TH_TYPE(n->suffix));
				s += strlen(s);
				break;
			case DCH_Q:
				if (!tm->tm_mon)
					break;
				sprintf(s, "%d", (tm->tm_mon - 1) / 3 + 1);
				if (S_THth(n->suffix))
					str_numth(s, s, S_TH_TYPE(n->suffix));
				s += strlen(s);
				break;
			case DCH_CC:
				if (is_interval)	/* straight calculation */
					i = tm->tm_year / 100;
				else
				{
					if (tm->tm_year > 0)
						/* Century 20 == 1901 - 2000 */
						i = (tm->tm_year - 1) / 100 + 1;
					else
						/* Century 6BC == 600BC - 501BC */
						i = tm->tm_year / 100 - 1;
				}
				if (i <= 99 && i >= -99)
					sprintf(s, "%0*d", S_FM(n->suffix) ? 0 : 2, i);
				else
					sprintf(s, "%d", i);
				if (S_THth(n->suffix))
					str_numth(s, s, S_TH_TYPE(n->suffix));
				s += strlen(s);
				break;
			case DCH_Y_YYY:
				i = ADJUST_YEAR(tm->tm_year, is_interval) / 1000;
				sprintf(s, "%d,%03d", i,
						ADJUST_YEAR(tm->tm_year, is_interval) - (i * 1000));
				if (S_THth(n->suffix))
					str_numth(s, s, S_TH_TYPE(n->suffix));
				s += strlen(s);
				break;
			case DCH_YYYY:
			case DCH_IYYY:
				sprintf(s, "%0*d",
						S_FM(n->suffix) ? 0 : 4,
						(n->key->id == DCH_YYYY ?
						 ADJUST_YEAR(tm->tm_year, is_interval) :
						 ADJUST_YEAR(date2isoyear(tm->tm_year,
												  tm->tm_mon,
												  tm->tm_mday),
									 is_interval)));
				if (S_THth(n->suffix))
					str_numth(s, s, S_TH_TYPE(n->suffix));
				s += strlen(s);
				break;
			case DCH_YYY:
			case DCH_IYY:
				sprintf(s, "%0*d",
						S_FM(n->suffix) ? 0 : 3,
						(n->key->id == DCH_YYY ?
						 ADJUST_YEAR(tm->tm_year, is_interval) :
						 ADJUST_YEAR(date2isoyear(tm->tm_year,
												  tm->tm_mon,
												  tm->tm_mday),
									 is_interval)) % 1000);
				if (S_THth(n->suffix))
					str_numth(s, s, S_TH_TYPE(n->suffix));
				s += strlen(s);
				break;
			case DCH_YY:
			case DCH_IY:
				sprintf(s, "%0*d",
						S_FM(n->suffix) ? 0 : 2,
						(n->key->id == DCH_YY ?
						 ADJUST_YEAR(tm->tm_year, is_interval) :
						 ADJUST_YEAR(date2isoyear(tm->tm_year,
												  tm->tm_mon,
												  tm->tm_mday),
									 is_interval)) % 100);
				if (S_THth(n->suffix))
					str_numth(s, s, S_TH_TYPE(n->suffix));
				s += strlen(s);
				break;
			case DCH_Y:
			case DCH_I:
				sprintf(s, "%1d",
						(n->key->id == DCH_Y ?
						 ADJUST_YEAR(tm->tm_year, is_interval) :
						 ADJUST_YEAR(date2isoyear(tm->tm_year,
												  tm->tm_mon,
												  tm->tm_mday),
									 is_interval)) % 10);
				if (S_THth(n->suffix))
					str_numth(s, s, S_TH_TYPE(n->suffix));
				s += strlen(s);
				break;
			case DCH_RM:
				if (!tm->tm_mon)
					break;
				sprintf(s, "%*s", S_FM(n->suffix) ? 0 : -4,
						rm_months_upper[MONTHS_PER_YEAR - tm->tm_mon]);
				s += strlen(s);
				break;
			case DCH_rm:
				if (!tm->tm_mon)
					break;
				sprintf(s, "%*s", S_FM(n->suffix) ? 0 : -4,
						rm_months_lower[MONTHS_PER_YEAR - tm->tm_mon]);
				s += strlen(s);
				break;
			case DCH_W:
				sprintf(s, "%d", (tm->tm_mday - 1) / 7 + 1);
				if (S_THth(n->suffix))
					str_numth(s, s, S_TH_TYPE(n->suffix));
				s += strlen(s);
				break;
			case DCH_J:
				sprintf(s, "%d", date2j(tm->tm_year, tm->tm_mon, tm->tm_mday));
				if (S_THth(n->suffix))
					str_numth(s, s, S_TH_TYPE(n->suffix));
				s += strlen(s);
				break;
		}
	}

	*s = '\0';
}