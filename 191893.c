from_char_seq_search(int *dest, char **src, const char *const * array, int type, int max,
					 FormatNode *node)
{
	int			len;

	*dest = seq_search(*src, array, type, max, &len);
	if (len <= 0)
	{
		char		copy[DCH_MAX_ITEM_SIZ + 1];

		Assert(max <= DCH_MAX_ITEM_SIZ);
		strlcpy(copy, *src, max + 1);

		ereport(ERROR,
				(errcode(ERRCODE_INVALID_DATETIME_FORMAT),
				 errmsg("invalid value \"%s\" for \"%s\"",
						copy, node->key->name),
				 errdetail("The given value did not match any of the allowed "
						   "values for this field.")));
	}
	*src += len;
	return len;
}