alloc_thread_static_data_helper (gpointer key, gpointer value, gpointer user)
{
	MonoInternalThread *thread = value;
	guint32 offset = GPOINTER_TO_UINT (user);

	mono_alloc_static_data (&(thread->static_data), offset, TRUE);
}