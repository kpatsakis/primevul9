mono_get_special_static_data (guint32 offset)
{
	return mono_get_special_static_data_for_thread (mono_thread_internal_current (), offset);
}