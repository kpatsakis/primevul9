get_caller (MonoMethod *m, gint32 no, gint32 ilo, gboolean managed, gpointer data)
{
	MonoMethod **dest = data;

	/* skip unmanaged frames */
	if (!managed)
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