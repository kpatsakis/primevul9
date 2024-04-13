get_executing (MonoMethod *m, gint32 no, gint32 ilo, gboolean managed, gpointer data)
{
	MonoMethod **dest = data;

	/* skip unmanaged frames */
	if (!managed)
		return FALSE;

	if (!(*dest)) {
		if (!strcmp (m->klass->name_space, "System.Reflection"))
			return FALSE;
		*dest = m;
		return TRUE;
	}
	return FALSE;
}