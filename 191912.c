from_char_set_int(int *dest, const int value, const FormatNode *node)
{
	if (*dest != 0 && *dest != value)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_DATETIME_FORMAT),
		   errmsg("conflicting values for \"%s\" field in formatting string",
				  node->key->name),
				 errdetail("This value contradicts a previous setting for "
						   "the same field type.")));
	*dest = value;
}