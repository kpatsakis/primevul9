NUMDesc_prepare(NUMDesc *num, FormatNode *n)
{
	if (n->type != NODE_TYPE_ACTION)
		return;

	/*
	 * In case of an error, we need to remove the numeric from the cache.  Use
	 * a PG_TRY block to ensure that this happens.
	 */
	PG_TRY();
	{
		if (IS_EEEE(num) && n->key->id != NUM_E)
			ereport(ERROR,
					(errcode(ERRCODE_SYNTAX_ERROR),
					 errmsg("\"EEEE\" must be the last pattern used")));

		switch (n->key->id)
		{
			case NUM_9:
				if (IS_BRACKET(num))
					ereport(ERROR,
							(errcode(ERRCODE_SYNTAX_ERROR),
							 errmsg("\"9\" must be ahead of \"PR\"")));
				if (IS_MULTI(num))
				{
					++num->multi;
					break;
				}
				if (IS_DECIMAL(num))
					++num->post;
				else
					++num->pre;
				break;

			case NUM_0:
				if (IS_BRACKET(num))
					ereport(ERROR,
							(errcode(ERRCODE_SYNTAX_ERROR),
							 errmsg("\"0\" must be ahead of \"PR\"")));
				if (!IS_ZERO(num) && !IS_DECIMAL(num))
				{
					num->flag |= NUM_F_ZERO;
					num->zero_start = num->pre + 1;
				}
				if (!IS_DECIMAL(num))
					++num->pre;
				else
					++num->post;

				num->zero_end = num->pre + num->post;
				break;

			case NUM_B:
				if (num->pre == 0 && num->post == 0 && (!IS_ZERO(num)))
					num->flag |= NUM_F_BLANK;
				break;

			case NUM_D:
				num->flag |= NUM_F_LDECIMAL;
				num->need_locale = TRUE;
				/* FALLTHROUGH */
			case NUM_DEC:
				if (IS_DECIMAL(num))
					ereport(ERROR,
							(errcode(ERRCODE_SYNTAX_ERROR),
							 errmsg("multiple decimal points")));
				if (IS_MULTI(num))
					ereport(ERROR,
							(errcode(ERRCODE_SYNTAX_ERROR),
					 errmsg("cannot use \"V\" and decimal point together")));
				num->flag |= NUM_F_DECIMAL;
				break;

			case NUM_FM:
				num->flag |= NUM_F_FILLMODE;
				break;

			case NUM_S:
				if (IS_LSIGN(num))
					ereport(ERROR,
							(errcode(ERRCODE_SYNTAX_ERROR),
							 errmsg("cannot use \"S\" twice")));
				if (IS_PLUS(num) || IS_MINUS(num) || IS_BRACKET(num))
					ereport(ERROR,
							(errcode(ERRCODE_SYNTAX_ERROR),
							 errmsg("cannot use \"S\" and \"PL\"/\"MI\"/\"SG\"/\"PR\" together")));
				if (!IS_DECIMAL(num))
				{
					num->lsign = NUM_LSIGN_PRE;
					num->pre_lsign_num = num->pre;
					num->need_locale = TRUE;
					num->flag |= NUM_F_LSIGN;
				}
				else if (num->lsign == NUM_LSIGN_NONE)
				{
					num->lsign = NUM_LSIGN_POST;
					num->need_locale = TRUE;
					num->flag |= NUM_F_LSIGN;
				}
				break;

			case NUM_MI:
				if (IS_LSIGN(num))
					ereport(ERROR,
							(errcode(ERRCODE_SYNTAX_ERROR),
							 errmsg("cannot use \"S\" and \"MI\" together")));
				num->flag |= NUM_F_MINUS;
				if (IS_DECIMAL(num))
					num->flag |= NUM_F_MINUS_POST;
				break;

			case NUM_PL:
				if (IS_LSIGN(num))
					ereport(ERROR,
							(errcode(ERRCODE_SYNTAX_ERROR),
							 errmsg("cannot use \"S\" and \"PL\" together")));
				num->flag |= NUM_F_PLUS;
				if (IS_DECIMAL(num))
					num->flag |= NUM_F_PLUS_POST;
				break;

			case NUM_SG:
				if (IS_LSIGN(num))
					ereport(ERROR,
							(errcode(ERRCODE_SYNTAX_ERROR),
							 errmsg("cannot use \"S\" and \"SG\" together")));
				num->flag |= NUM_F_MINUS;
				num->flag |= NUM_F_PLUS;
				break;

			case NUM_PR:
				if (IS_LSIGN(num) || IS_PLUS(num) || IS_MINUS(num))
					ereport(ERROR,
							(errcode(ERRCODE_SYNTAX_ERROR),
							 errmsg("cannot use \"PR\" and \"S\"/\"PL\"/\"MI\"/\"SG\" together")));
				num->flag |= NUM_F_BRACKET;
				break;

			case NUM_rn:
			case NUM_RN:
				num->flag |= NUM_F_ROMAN;
				break;

			case NUM_L:
			case NUM_G:
				num->need_locale = TRUE;
				break;

			case NUM_V:
				if (IS_DECIMAL(num))
					ereport(ERROR,
							(errcode(ERRCODE_SYNTAX_ERROR),
					 errmsg("cannot use \"V\" and decimal point together")));
				num->flag |= NUM_F_MULTI;
				break;

			case NUM_E:
				if (IS_EEEE(num))
					ereport(ERROR,
							(errcode(ERRCODE_SYNTAX_ERROR),
							 errmsg("cannot use \"EEEE\" twice")));
				if (IS_BLANK(num) || IS_FILLMODE(num) || IS_LSIGN(num) ||
					IS_BRACKET(num) || IS_MINUS(num) || IS_PLUS(num) ||
					IS_ROMAN(num) || IS_MULTI(num))
					ereport(ERROR,
							(errcode(ERRCODE_SYNTAX_ERROR),
					   errmsg("\"EEEE\" is incompatible with other formats"),
							 errdetail("\"EEEE\" may only be used together with digit and decimal point patterns.")));
				num->flag |= NUM_F_EEEE;
				break;
		}
	}
	PG_CATCH();
	{
		NUM_cache_remove(last_NUMCacheEntry);
		PG_RE_THROW();
	}
	PG_END_TRY();


	return;
}