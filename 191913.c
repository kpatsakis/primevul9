NUM_numpart_from_char(NUMProc *Np, int id, int input_len)
{
	bool		isread = FALSE;

#ifdef DEBUG_TO_FROM_CHAR
	elog(DEBUG_elog_output, " --- scan start --- id=%s",
		 (id == NUM_0 || id == NUM_9) ? "NUM_0/9" : id == NUM_DEC ? "NUM_DEC" : "???");
#endif

	if (*Np->inout_p == ' ')
		Np->inout_p++;

#define OVERLOAD_TEST	(Np->inout_p >= Np->inout + input_len)
#define AMOUNT_TEST(_s) (input_len-(Np->inout_p-Np->inout) >= _s)

	if (*Np->inout_p == ' ')
		Np->inout_p++;

	if (OVERLOAD_TEST)
		return;

	/*
	 * read sign before number
	 */
	if (*Np->number == ' ' && (id == NUM_0 || id == NUM_9) &&
		(Np->read_pre + Np->read_post) == 0)
	{
#ifdef DEBUG_TO_FROM_CHAR
		elog(DEBUG_elog_output, "Try read sign (%c), locale positive: %s, negative: %s",
			 *Np->inout_p, Np->L_positive_sign, Np->L_negative_sign);
#endif

		/*
		 * locale sign
		 */
		if (IS_LSIGN(Np->Num) && Np->Num->lsign == NUM_LSIGN_PRE)
		{
			int			x = 0;

#ifdef DEBUG_TO_FROM_CHAR
			elog(DEBUG_elog_output, "Try read locale pre-sign (%c)", *Np->inout_p);
#endif
			if ((x = strlen(Np->L_negative_sign)) &&
				AMOUNT_TEST(x) &&
				strncmp(Np->inout_p, Np->L_negative_sign, x) == 0)
			{
				Np->inout_p += x;
				*Np->number = '-';
			}
			else if ((x = strlen(Np->L_positive_sign)) &&
					 AMOUNT_TEST(x) &&
					 strncmp(Np->inout_p, Np->L_positive_sign, x) == 0)
			{
				Np->inout_p += x;
				*Np->number = '+';
			}
		}
		else
		{
#ifdef DEBUG_TO_FROM_CHAR
			elog(DEBUG_elog_output, "Try read simple sign (%c)", *Np->inout_p);
#endif

			/*
			 * simple + - < >
			 */
			if (*Np->inout_p == '-' || (IS_BRACKET(Np->Num) &&
										*Np->inout_p == '<'))
			{
				*Np->number = '-';		/* set - */
				Np->inout_p++;
			}
			else if (*Np->inout_p == '+')
			{
				*Np->number = '+';		/* set + */
				Np->inout_p++;
			}
		}
	}

	if (OVERLOAD_TEST)
		return;

#ifdef DEBUG_TO_FROM_CHAR
	elog(DEBUG_elog_output, "Scan for numbers (%c), current number: '%s'", *Np->inout_p, Np->number);
#endif

	/*
	 * read digit or decimal point
	 */
	if (isdigit((unsigned char) *Np->inout_p))
	{
		if (Np->read_dec && Np->read_post == Np->Num->post)
			return;

		*Np->number_p = *Np->inout_p;
		Np->number_p++;

		if (Np->read_dec)
			Np->read_post++;
		else
			Np->read_pre++;

		isread = TRUE;

#ifdef DEBUG_TO_FROM_CHAR
		elog(DEBUG_elog_output, "Read digit (%c)", *Np->inout_p);
#endif
	}
	else if (IS_DECIMAL(Np->Num) && Np->read_dec == FALSE)
	{
		/*
		 * We need not test IS_LDECIMAL(Np->Num) explicitly here, because
		 * Np->decimal is always just "." if we don't have a D format token.
		 * So we just unconditionally match to Np->decimal.
		 */
		int			x = strlen(Np->decimal);

#ifdef DEBUG_TO_FROM_CHAR
		elog(DEBUG_elog_output, "Try read decimal point (%c)",
			 *Np->inout_p);
#endif
		if (x && AMOUNT_TEST(x) && strncmp(Np->inout_p, Np->decimal, x) == 0)
		{
			Np->inout_p += x - 1;
			*Np->number_p = '.';
			Np->number_p++;
			Np->read_dec = TRUE;
			isread = TRUE;
		}
	}

	if (OVERLOAD_TEST)
		return;

	/*
	 * Read sign behind "last" number
	 *
	 * We need sign detection because determine exact position of post-sign is
	 * difficult:
	 *
	 * FM9999.9999999S	   -> 123.001- 9.9S			   -> .5- FM9.999999MI ->
	 * 5.01-
	 */
	if (*Np->number == ' ' && Np->read_pre + Np->read_post > 0)
	{
		/*
		 * locale sign (NUM_S) is always anchored behind a last number, if: -
		 * locale sign expected - last read char was NUM_0/9 or NUM_DEC - and
		 * next char is not digit
		 */
		if (IS_LSIGN(Np->Num) && isread &&
			(Np->inout_p + 1) <= Np->inout + input_len &&
			!isdigit((unsigned char) *(Np->inout_p + 1)))
		{
			int			x;
			char	   *tmp = Np->inout_p++;

#ifdef DEBUG_TO_FROM_CHAR
			elog(DEBUG_elog_output, "Try read locale post-sign (%c)", *Np->inout_p);
#endif
			if ((x = strlen(Np->L_negative_sign)) &&
				AMOUNT_TEST(x) &&
				strncmp(Np->inout_p, Np->L_negative_sign, x) == 0)
			{
				Np->inout_p += x - 1;	/* -1 .. NUM_processor() do inout_p++ */
				*Np->number = '-';
			}
			else if ((x = strlen(Np->L_positive_sign)) &&
					 AMOUNT_TEST(x) &&
					 strncmp(Np->inout_p, Np->L_positive_sign, x) == 0)
			{
				Np->inout_p += x - 1;	/* -1 .. NUM_processor() do inout_p++ */
				*Np->number = '+';
			}
			if (*Np->number == ' ')
				/* no sign read */
				Np->inout_p = tmp;
		}

		/*
		 * try read non-locale sign, it's happen only if format is not exact
		 * and we cannot determine sign position of MI/PL/SG, an example:
		 *
		 * FM9.999999MI			   -> 5.01-
		 *
		 * if (.... && IS_LSIGN(Np->Num)==FALSE) prevents read wrong formats
		 * like to_number('1 -', '9S') where sign is not anchored to last
		 * number.
		 */
		else if (isread == FALSE && IS_LSIGN(Np->Num) == FALSE &&
				 (IS_PLUS(Np->Num) || IS_MINUS(Np->Num)))
		{
#ifdef DEBUG_TO_FROM_CHAR
			elog(DEBUG_elog_output, "Try read simple post-sign (%c)", *Np->inout_p);
#endif

			/*
			 * simple + -
			 */
			if (*Np->inout_p == '-' || *Np->inout_p == '+')
				/* NUM_processor() do inout_p++ */
				*Np->number = *Np->inout_p;
		}
	}
}