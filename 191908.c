str_initcap(const char *buff, size_t nbytes, Oid collid)
{
	char	   *result;
	int			wasalnum = false;

	if (!buff)
		return NULL;

	/* C/POSIX collations use this path regardless of database encoding */
	if (lc_ctype_is_c(collid))
	{
		result = asc_initcap(buff, nbytes);
	}
#ifdef USE_WIDE_UPPER_LOWER
	else if (pg_database_encoding_max_length() > 1)
	{
		pg_locale_t mylocale = 0;
		wchar_t    *workspace;
		size_t		curr_char;
		size_t		result_size;

		if (collid != DEFAULT_COLLATION_OID)
		{
			if (!OidIsValid(collid))
			{
				/*
				 * This typically means that the parser could not resolve a
				 * conflict of implicit collations, so report it that way.
				 */
				ereport(ERROR,
						(errcode(ERRCODE_INDETERMINATE_COLLATION),
						 errmsg("could not determine which collation to use for initcap() function"),
						 errhint("Use the COLLATE clause to set the collation explicitly.")));
			}
			mylocale = pg_newlocale_from_collation(collid);
		}

		/* Overflow paranoia */
		if ((nbytes + 1) > (INT_MAX / sizeof(wchar_t)))
			ereport(ERROR,
					(errcode(ERRCODE_OUT_OF_MEMORY),
					 errmsg("out of memory")));

		/* Output workspace cannot have more codes than input bytes */
		workspace = (wchar_t *) palloc((nbytes + 1) * sizeof(wchar_t));

		char2wchar(workspace, nbytes + 1, buff, nbytes, mylocale);

		for (curr_char = 0; workspace[curr_char] != 0; curr_char++)
		{
#ifdef HAVE_LOCALE_T
			if (mylocale)
			{
				if (wasalnum)
					workspace[curr_char] = towlower_l(workspace[curr_char], mylocale);
				else
					workspace[curr_char] = towupper_l(workspace[curr_char], mylocale);
				wasalnum = iswalnum_l(workspace[curr_char], mylocale);
			}
			else
#endif
			{
				if (wasalnum)
					workspace[curr_char] = towlower(workspace[curr_char]);
				else
					workspace[curr_char] = towupper(workspace[curr_char]);
				wasalnum = iswalnum(workspace[curr_char]);
			}
		}

		/* Make result large enough; case change might change number of bytes */
		result_size = curr_char * pg_database_encoding_max_length() + 1;
		result = palloc(result_size);

		wchar2char(result, workspace, result_size, mylocale);
		pfree(workspace);
	}
#endif   /* USE_WIDE_UPPER_LOWER */
	else
	{
#ifdef HAVE_LOCALE_T
		pg_locale_t mylocale = 0;
#endif
		char	   *p;

		if (collid != DEFAULT_COLLATION_OID)
		{
			if (!OidIsValid(collid))
			{
				/*
				 * This typically means that the parser could not resolve a
				 * conflict of implicit collations, so report it that way.
				 */
				ereport(ERROR,
						(errcode(ERRCODE_INDETERMINATE_COLLATION),
						 errmsg("could not determine which collation to use for initcap() function"),
						 errhint("Use the COLLATE clause to set the collation explicitly.")));
			}
#ifdef HAVE_LOCALE_T
			mylocale = pg_newlocale_from_collation(collid);
#endif
		}

		result = pnstrdup(buff, nbytes);

		/*
		 * Note: we assume that toupper_l()/tolower_l() will not be so broken
		 * as to need guard tests.  When using the default collation, we apply
		 * the traditional Postgres behavior that forces ASCII-style treatment
		 * of I/i, but in non-default collations you get exactly what the
		 * collation says.
		 */
		for (p = result; *p; p++)
		{
#ifdef HAVE_LOCALE_T
			if (mylocale)
			{
				if (wasalnum)
					*p = tolower_l((unsigned char) *p, mylocale);
				else
					*p = toupper_l((unsigned char) *p, mylocale);
				wasalnum = isalnum_l((unsigned char) *p, mylocale);
			}
			else
#endif
			{
				if (wasalnum)
					*p = pg_tolower((unsigned char) *p);
				else
					*p = pg_toupper((unsigned char) *p);
				wasalnum = isalnum((unsigned char) *p);
			}
		}
	}

	return result;
}