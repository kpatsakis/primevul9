get_caller_no_reflection (MonoMethod *m, gint32 no, gint32 ilo, gboolean managed, gpointer data)
{
	MonoMethod **dest = data;

	/* skip unmanaged frames */
	if (!managed)
		return FALSE;

	if (m->wrapper_type != MONO_WRAPPER_NONE)
		return FALSE;

	if (m->klass->image == mono_defaults.corlib && !strcmp (m->klass->name_space, "System.Reflection"))
		return FALSE;

	if (m == *dest) {
		*dest = NULL;
		return FALSE;
	}
	if (!(*dest)) {
		*dest = m;
		return TRUE;
	}
	return FALSE;
}