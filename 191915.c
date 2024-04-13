is_next_separator(FormatNode *n)
{
	if (n->type == NODE_TYPE_END)
		return FALSE;

	if (n->type == NODE_TYPE_ACTION && S_THth(n->suffix))
		return TRUE;

	/*
	 * Next node
	 */
	n++;

	/* end of format string is treated like a non-digit separator */
	if (n->type == NODE_TYPE_END)
		return TRUE;

	if (n->type == NODE_TYPE_ACTION)
	{
		if (n->key->is_digit)
			return FALSE;

		return TRUE;
	}
	else if (isdigit((unsigned char) n->character))
		return FALSE;

	return TRUE;				/* some non-digit input (separator) */
}