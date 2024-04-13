ref_stack_push (RefStack *rs, gpointer ptr)
{
	g_assert (rs != NULL);

	if (rs->bottom >= rs->allocated) {
		rs->refs = g_realloc (rs->refs, rs->allocated * 2 * sizeof (gpointer) + 1);
		rs->allocated <<= 1;
		rs->refs [rs->allocated] = NULL;
	}
	rs->refs [rs->bottom++] = ptr;
}