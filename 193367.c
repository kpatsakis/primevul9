is_running_protected_wrapper (void)
{
	gboolean found = FALSE;
	mono_stack_walk (find_wrapper, &found);
	return found;
}