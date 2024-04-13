ref_stack_find (RefStack *rs, gpointer ptr)
{
	gpointer *refs;

	if (rs == NULL)
		return FALSE;

	for (refs = rs->refs; refs && *refs; refs++) {
		if (*refs == ptr)
			return TRUE;
	}
	return FALSE;
}