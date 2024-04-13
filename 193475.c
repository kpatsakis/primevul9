ref_stack_destroy (gpointer ptr)
{
	RefStack *rs = ptr;

	if (rs != NULL) {
		g_free (rs->refs);
		g_free (rs);
	}
}