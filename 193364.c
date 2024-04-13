ref_stack_new (gint initial_size)
{
	RefStack *rs;

	initial_size = MAX (initial_size, 16) + 1;
	rs = g_new0 (RefStack, 1);
	rs->refs = g_new0 (gpointer, initial_size);
	rs->allocated = initial_size;
	return rs;
}