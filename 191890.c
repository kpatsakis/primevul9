from_char_parse_int_len(int *dest, char **src, const int len, FormatNode *node)
{
	long		result;
	char		copy[DCH_MAX_ITEM_SIZ + 1];
	char	   *init = *src;
	int			used;

	/*
	 * Skip any whitespace before parsing the integer.
	 */
	*src += strspace_len(*src);

	Assert(len <= DCH_MAX_ITEM_SIZ);
	used = (int) strlcpy(copy, *src, len + 1);

	if (S_FM(node->suffix) || is_next_separator(node))
	{
		/*
		 * This node is in Fill Mode, or the next node is known to be a
		 * non-digit value, so we just slurp as many characters as we can get.
		 */
		errno = 0;
		result = strtol(init, src, 10);
	}
	else
	{
		/*
		 * We need to pull exactly the number of characters given in 'len' out
		 * of the string, and convert those.
		 */
		char	   *last;

		if (used < len)
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_DATETIME_FORMAT),
				errmsg("source string too short for \"%s\" formatting field",
					   node->key->name),
					 errdetail("Field requires %d characters, but only %d "
							   "remain.",
							   len, used),
					 errhint("If your source string is not fixed-width, try "
							 "using the \"FM\" modifier.")));

		errno = 0;
		result = strtol(copy, &last, 10);
		used = last - copy;

		if (used > 0 && used < len)
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_DATETIME_FORMAT),
					 errmsg("invalid value \"%s\" for \"%s\"",
							copy, node->key->name),
					 errdetail("Field requires %d characters, but only %d "
							   "could be parsed.", len, used),
					 errhint("If your source string is not fixed-width, try "
							 "using the \"FM\" modifier.")));

		*src += used;
	}

	if (*src == init)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_DATETIME_FORMAT),
				 errmsg("invalid value \"%s\" for \"%s\"",
						copy, node->key->name),
				 errdetail("Value must be an integer.")));

	if (errno == ERANGE || result < INT_MIN || result > INT_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
				 errmsg("value for \"%s\" in source string is out of range",
						node->key->name),
				 errdetail("Value must be in the range %d to %d.",
						   INT_MIN, INT_MAX)));

	if (dest != NULL)
		from_char_set_int(dest, (int) result, node);
	return *src - init;
}