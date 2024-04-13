find_wrapper (MonoMethod *m, gint no, gint ilo, gboolean managed, gpointer data)
{
	if (managed)
		return TRUE;

	if (m->wrapper_type == MONO_WRAPPER_RUNTIME_INVOKE ||
		m->wrapper_type == MONO_WRAPPER_XDOMAIN_INVOKE ||
		m->wrapper_type == MONO_WRAPPER_XDOMAIN_DISPATCH) 
	{
		*((gboolean*)data) = TRUE;
		return TRUE;
	}
	return FALSE;
}