ref_stack_pop (RefStack *rs)
{
	if (rs == NULL || rs->bottom == 0)
		return;

	rs->bottom--;
	rs->refs [rs->bottom] = NULL;
}