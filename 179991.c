property_accessor_nonpublic (MonoMethod* accessor, gboolean start_klass)
{
	if (!accessor)
		return FALSE;

	return method_nonpublic (accessor, start_klass);
}