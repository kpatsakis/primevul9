NUM_processor(FormatNode *node, NUMDesc *Num, char *inout,
			  char *number, int from_char_input_len, int to_char_out_pre_spaces,
			  int sign, bool is_to_char, Oid collid)
{
	FormatNode *n;
	NUMProc		_Np,
			   *Np = &_Np;

	MemSet(Np, 0, sizeof(NUMProc));

	Np->Num = Num;
	Np->is_to_char = is_to_char;
	Np->number = number;
	Np->inout = inout;
	Np->last_relevant = NULL;
	Np->read_post = 0;
	Np->read_pre = 0;
	Np->read_dec = FALSE;

	if (Np->Num->zero_start)
		--Np->Num->zero_start;

	if (IS_EEEE(Np->Num))
	{
		if (!Np->is_to_char)
			ereport(ERROR,
					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
					 errmsg("\"EEEE\" not supported for input")));
		return strcpy(inout, number);
	}

	/*
	 * Roman correction
	 */
	if (IS_ROMAN(Np->Num))
	{
		if (!Np->is_to_char)
			ereport(ERROR,
					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
					 errmsg("\"RN\" not supported for input")));

		Np->Num->lsign = Np->Num->pre_lsign_num = Np->Num->post =
			Np->Num->pre = Np->out_pre_spaces = Np->sign = 0;

		if (IS_FILLMODE(Np->Num))
		{
			Np->Num->flag = 0;
			Np->Num->flag |= NUM_F_FILLMODE;
		}
		else
			Np->Num->flag = 0;
		Np->Num->flag |= NUM_F_ROMAN;
	}

	/*
	 * Sign
	 */
	if (is_to_char)
	{
		Np->sign = sign;

		/* MI/PL/SG - write sign itself and not in number */
		if (IS_PLUS(Np->Num) || IS_MINUS(Np->Num))
		{
			if (IS_PLUS(Np->Num) && IS_MINUS(Np->Num) == FALSE)
				Np->sign_wrote = FALSE; /* need sign */
			else
				Np->sign_wrote = TRUE;	/* needn't sign */
		}
		else
		{
			if (Np->sign != '-')
			{
				if (IS_BRACKET(Np->Num) && IS_FILLMODE(Np->Num))
					Np->Num->flag &= ~NUM_F_BRACKET;
				if (IS_MINUS(Np->Num))
					Np->Num->flag &= ~NUM_F_MINUS;
			}
			else if (Np->sign != '+' && IS_PLUS(Np->Num))
				Np->Num->flag &= ~NUM_F_PLUS;

			if (Np->sign == '+' && IS_FILLMODE(Np->Num) && IS_LSIGN(Np->Num) == FALSE)
				Np->sign_wrote = TRUE;	/* needn't sign */
			else
				Np->sign_wrote = FALSE; /* need sign */

			if (Np->Num->lsign == NUM_LSIGN_PRE && Np->Num->pre == Np->Num->pre_lsign_num)
				Np->Num->lsign = NUM_LSIGN_POST;
		}
	}
	else
		Np->sign = FALSE;

	/*
	 * Count
	 */
	Np->num_count = Np->Num->post + Np->Num->pre - 1;

	if (is_to_char)
	{
		Np->out_pre_spaces = to_char_out_pre_spaces;

		if (IS_FILLMODE(Np->Num) && IS_DECIMAL(Np->Num))
		{
			Np->last_relevant = get_last_relevant_decnum(Np->number);

			/*
			 * If any '0' specifiers are present, make sure we don't strip
			 * those digits.
			 */
			if (Np->last_relevant && Np->Num->zero_end > Np->out_pre_spaces)
			{
				char	   *last_zero;

				last_zero = Np->number + (Np->Num->zero_end - Np->out_pre_spaces);
				if (Np->last_relevant < last_zero)
					Np->last_relevant = last_zero;
			}
		}

		if (Np->sign_wrote == FALSE && Np->out_pre_spaces == 0)
			++Np->num_count;
	}
	else
	{
		Np->out_pre_spaces = 0;
		*Np->number = ' ';		/* sign space */
		*(Np->number + 1) = '\0';
	}

	Np->num_in = 0;
	Np->num_curr = 0;

#ifdef DEBUG_TO_FROM_CHAR
	elog(DEBUG_elog_output,
		 "\n\tSIGN: '%c'\n\tNUM: '%s'\n\tPRE: %d\n\tPOST: %d\n\tNUM_COUNT: %d\n\tNUM_PRE: %d\n\tSIGN_WROTE: %s\n\tZERO: %s\n\tZERO_START: %d\n\tZERO_END: %d\n\tLAST_RELEVANT: %s\n\tBRACKET: %s\n\tPLUS: %s\n\tMINUS: %s\n\tFILLMODE: %s\n\tROMAN: %s\n\tEEEE: %s",
		 Np->sign,
		 Np->number,
		 Np->Num->pre,
		 Np->Num->post,
		 Np->num_count,
		 Np->out_pre_spaces,
		 Np->sign_wrote ? "Yes" : "No",
		 IS_ZERO(Np->Num) ? "Yes" : "No",
		 Np->Num->zero_start,
		 Np->Num->zero_end,
		 Np->last_relevant ? Np->last_relevant : "<not set>",
		 IS_BRACKET(Np->Num) ? "Yes" : "No",
		 IS_PLUS(Np->Num) ? "Yes" : "No",
		 IS_MINUS(Np->Num) ? "Yes" : "No",
		 IS_FILLMODE(Np->Num) ? "Yes" : "No",
		 IS_ROMAN(Np->Num) ? "Yes" : "No",
		 IS_EEEE(Np->Num) ? "Yes" : "No"
		);
#endif

	/*
	 * Locale
	 */
	NUM_prepare_locale(Np);

	/*
	 * Processor direct cycle
	 */
	if (Np->is_to_char)
		Np->number_p = Np->number;
	else
		Np->number_p = Np->number + 1;	/* first char is space for sign */

	for (n = node, Np->inout_p = Np->inout; n->type != NODE_TYPE_END; n++)
	{
		if (!Np->is_to_char)
		{
			/*
			 * Check non-string inout end
			 */
			if (Np->inout_p >= Np->inout + from_char_input_len)
				break;
		}

		/*
		 * Format pictures actions
		 */
		if (n->type == NODE_TYPE_ACTION)
		{
			/*
			 * Create/reading digit/zero/blank/sing
			 *
			 * 'NUM_S' note: The locale sign is anchored to number and we
			 * read/write it when we work with first or last number
			 * (NUM_0/NUM_9). This is reason why NUM_S missing in follow
			 * switch().
			 */
			switch (n->key->id)
			{
				case NUM_9:
				case NUM_0:
				case NUM_DEC:
				case NUM_D:
					if (Np->is_to_char)
					{
						NUM_numpart_to_char(Np, n->key->id);
						continue;		/* for() */
					}
					else
					{
						NUM_numpart_from_char(Np, n->key->id, from_char_input_len);
						break;	/* switch() case: */
					}

				case NUM_COMMA:
					if (Np->is_to_char)
					{
						if (!Np->num_in)
						{
							if (IS_FILLMODE(Np->Num))
								continue;
							else
								*Np->inout_p = ' ';
						}
						else
							*Np->inout_p = ',';
					}
					else
					{
						if (!Np->num_in)
						{
							if (IS_FILLMODE(Np->Num))
								continue;
						}
					}
					break;

				case NUM_G:
					if (Np->is_to_char)
					{
						if (!Np->num_in)
						{
							if (IS_FILLMODE(Np->Num))
								continue;
							else
							{
								int			x = strlen(Np->L_thousands_sep);

								memset(Np->inout_p, ' ', x);
								Np->inout_p += x - 1;
							}
						}
						else
						{
							strcpy(Np->inout_p, Np->L_thousands_sep);
							Np->inout_p += strlen(Np->inout_p) - 1;
						}
					}
					else
					{
						if (!Np->num_in)
						{
							if (IS_FILLMODE(Np->Num))
								continue;
						}
						Np->inout_p += strlen(Np->L_thousands_sep) - 1;
					}
					break;

				case NUM_L:
					if (Np->is_to_char)
					{
						strcpy(Np->inout_p, Np->L_currency_symbol);
						Np->inout_p += strlen(Np->inout_p) - 1;
					}
					else
						Np->inout_p += strlen(Np->L_currency_symbol) - 1;
					break;

				case NUM_RN:
					if (IS_FILLMODE(Np->Num))
					{
						strcpy(Np->inout_p, Np->number_p);
						Np->inout_p += strlen(Np->inout_p) - 1;
					}
					else
					{
						sprintf(Np->inout_p, "%15s", Np->number_p);
						Np->inout_p += strlen(Np->inout_p) - 1;
					}
					break;

				case NUM_rn:
					if (IS_FILLMODE(Np->Num))
					{
						strcpy(Np->inout_p, asc_tolower_z(Np->number_p));
						Np->inout_p += strlen(Np->inout_p) - 1;
					}
					else
					{
						sprintf(Np->inout_p, "%15s", asc_tolower_z(Np->number_p));
						Np->inout_p += strlen(Np->inout_p) - 1;
					}
					break;

				case NUM_th:
					if (IS_ROMAN(Np->Num) || *Np->number == '#' ||
						Np->sign == '-' || IS_DECIMAL(Np->Num))
						continue;

					if (Np->is_to_char)
						strcpy(Np->inout_p, get_th(Np->number, TH_LOWER));
					Np->inout_p += 1;
					break;

				case NUM_TH:
					if (IS_ROMAN(Np->Num) || *Np->number == '#' ||
						Np->sign == '-' || IS_DECIMAL(Np->Num))
						continue;

					if (Np->is_to_char)
						strcpy(Np->inout_p, get_th(Np->number, TH_UPPER));
					Np->inout_p += 1;
					break;

				case NUM_MI:
					if (Np->is_to_char)
					{
						if (Np->sign == '-')
							*Np->inout_p = '-';
						else if (IS_FILLMODE(Np->Num))
							continue;
						else
							*Np->inout_p = ' ';
					}
					else
					{
						if (*Np->inout_p == '-')
							*Np->number = '-';
					}
					break;

				case NUM_PL:
					if (Np->is_to_char)
					{
						if (Np->sign == '+')
							*Np->inout_p = '+';
						else if (IS_FILLMODE(Np->Num))
							continue;
						else
							*Np->inout_p = ' ';
					}
					else
					{
						if (*Np->inout_p == '+')
							*Np->number = '+';
					}
					break;

				case NUM_SG:
					if (Np->is_to_char)
						*Np->inout_p = Np->sign;

					else
					{
						if (*Np->inout_p == '-')
							*Np->number = '-';
						else if (*Np->inout_p == '+')
							*Np->number = '+';
					}
					break;


				default:
					continue;
					break;
			}
		}
		else
		{
			/*
			 * Remove to output char from input in TO_CHAR
			 */
			if (Np->is_to_char)
				*Np->inout_p = n->character;
		}
		Np->inout_p++;
	}

	if (Np->is_to_char)
	{
		*Np->inout_p = '\0';
		return Np->inout;
	}
	else
	{
		if (*(Np->number_p - 1) == '.')
			*(Np->number_p - 1) = '\0';
		else
			*Np->number_p = '\0';

		/*
		 * Correction - precision of dec. number
		 */
		Np->Num->post = Np->read_post;

#ifdef DEBUG_TO_FROM_CHAR
		elog(DEBUG_elog_output, "TO_NUMBER (number): '%s'", Np->number);
#endif
		return Np->number;
	}
}